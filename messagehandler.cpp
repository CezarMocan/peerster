#include "messagehandler.h"

#include <QVariantMap>

MessageHandler::MessageHandler(NetSocket *sock, QString localhostName) {
    this->sock = sock;
    this->localhostName = localhostName;
}

void MessageHandler::parseRumorMessage(QVariantMap textVariantMap, Peer currentPeer) {
    //qDebug() << "Received rumor: " << sender << senderPort;
    quint32 lastIp;
    quint16 lastPort;
    QString receivedText = NULL;
    if (textVariantMap.contains(sock->DEFAULT_TEXT_KEY))
        receivedText = textVariantMap[sock->DEFAULT_TEXT_KEY].toString();

    bool isDirect = 1;
    if (textVariantMap.contains(sock->DEFAULT_LAST_IP_KEY)) {
        lastIp = textVariantMap[sock->DEFAULT_LAST_IP_KEY].toUInt();
        lastPort = textVariantMap[sock->DEFAULT_LAST_PORT_KEY].toUInt();
        Peer lastPeer(QHostAddress(lastIp), lastPort);

        emit handlerAddPeerToList(currentPeer);
        //parent->addPeerToList(lastPeer);
        //qDebug() << "Last peer:" << lastIp << lastPort;
        isDirect = 0;
    }

    QString originName = textVariantMap[sock->DEFAULT_ORIGIN_KEY].toString();
    quint32 seqNo = textVariantMap[sock->DEFAULT_SEQ_NO_KEY].toUInt();

    emit(gotNewRumorMessage(currentPeer, originName, receivedText, seqNo, isDirect));
}

void MessageHandler::parseStatusMessage(QVariantMap textVariantMap, Peer currentPeer) {
    // parse want map send stuff to the other machine
    QVariantMap wantMap = textVariantMap[sock->DEFAULT_WANT_KEY].toMap();
    QVariantMap::iterator it;

    // Perform 2 iterations; in first one just check if I have anything new, send it if I do and return
    // in the second one (which I get to only after I send everything i have that my peer doesn't) I send my status map
    // if I find anything that he has and I don't;
    // by doing this the chances of an infinite loop of status messages between me and my peer should be less
    // actually 0 if he does the same, but I can't rely on that

    for (it = wantMap.begin(); it != wantMap.end(); ++it) {
        QString gossipAboutName = it.key();
        int size = it.value().toInt();

        if (!messages.contains(gossipAboutName)) {
            continue;
        }

        if (messages[gossipAboutName].size() + 1 > size) { // I have more info than he does from this host, I'll send him message and quit
            sock->sendMessage(gossipAboutName, messages[gossipAboutName][size - 1], size, currentPeer, 0, 0);
            return;
        }
    }

    QMap<QString, QVector<QString> >::iterator it2;
    for (it2 = messages.begin(); it2 != messages.end(); ++it2) {
        if (messages[it2.key()].size() == 0)
            continue;
        if (!wantMap.contains(it2.key())) {
            sock->sendMessage(it2.key(), messages[it2.key()][0], 1, currentPeer, 0, 0);
            return;
        }
    }

    for (it = wantMap.begin(); it != wantMap.end(); ++it) {
        QString gossipAboutName = it.key();
        int size = it.value().toInt();

        if (!messages.contains(it.key())) {
            sock->sendStatus(currentPeer, messages);
        }
        if (messages[gossipAboutName].size() + 1 < size) { // Peer has more info than I do, I'll send dat bitch a status map
            sock->sendStatus(currentPeer, messages);
            return;
        }
    }
}

void MessageHandler::parsePrivateMessage(QVariantMap textVariantMap, Peer currentPeer) {
    QString dest = textVariantMap[sock->DEFAULT_DEST_KEY].toString();
    QString message = textVariantMap[sock->DEFAULT_TEXT_KEY].toString();
    quint32 hopLimit = (textVariantMap[sock->DEFAULT_HOP_LIMIT_KEY].toInt()) - 1;
    QString originName = sock->RECEIVED_MESSAGE_WINDOW;
    if (textVariantMap.contains(sock->DEFAULT_ORIGIN_KEY))
        originName = textVariantMap[sock->DEFAULT_ORIGIN_KEY].toString();

    qDebug() << "Received private message from: " << originName;

    if (dest == localhostName) {
        emit(gotNewPrivateMessage(currentPeer, originName, message, sock->RECEIVE_PRIVATE, hopLimit));
    } else {
        // Drop a message if I'm not the destination and hopLimit got to be 0
        if (!checkForward(hopLimit, dest))
            return;

        if (originName == sock->RECEIVED_MESSAGE_WINDOW)
            sock->sendPrivateMessage(NULL, dest, message, routingMap[dest], hopLimit);
        else
            sock->sendPrivateMessage(originName, dest, message, routingMap[dest], hopLimit);
    }

    return;
}

void MessageHandler::parseBlockRequest(QVariantMap textVariantMap, Peer currentPeer) {
    //printMap(textVariantMap);
    QString dest = textVariantMap[sock->DEFAULT_DEST_KEY].toString();
    quint32 hopLimit = (textVariantMap[sock->DEFAULT_HOP_LIMIT_KEY].toInt()) - 1;
    QString originName = textVariantMap[sock->DEFAULT_ORIGIN_KEY].toString();
    QByteArray requestedBlock = textVariantMap[sock->DEFAULT_BLOCK_REQUEST_KEY].toByteArray();

    qDebug() << "Received block request with destination " << dest;

    if (dest == localhostName) {
        qDebug() << "Block request is for me!";
        emit(gotNewBlockRequest(dest, hopLimit, originName, requestedBlock));
    } else {
        qDebug() << "Block request is not for me!";
        if (!checkForward(hopLimit, dest))
            return;

        sock->sendBlockRequest(originName, dest, requestedBlock, routingMap[dest], hopLimit);
    }
}

void MessageHandler::parseBlockResponse(QVariantMap textVariantMap, Peer currentPeer) {
    //printMap(textVariantMap);
    QString dest = textVariantMap[sock->DEFAULT_DEST_KEY].toString();
    quint32 hopLimit = (textVariantMap[sock->DEFAULT_HOP_LIMIT_KEY].toInt()) - 1;
    QString originName = textVariantMap[sock->DEFAULT_ORIGIN_KEY].toString();
    QByteArray repliedBlock = textVariantMap[sock->DEFAULT_BLOCK_REPLY_KEY].toByteArray();
    QByteArray data = textVariantMap[sock->DEFAULT_DATA_KEY].toByteArray();

    qDebug() << "Got new block response for destination = " << dest;

    if (dest == localhostName) {
        emit(gotNewBlockResponse(originName, repliedBlock, data));
    } else {
        if (!checkForward(hopLimit, dest))
            return;
        if (!checkHash(data, repliedBlock)) // Drop
            return;

        sock->sendBlockReply(originName, dest, repliedBlock, data, routingMap[dest], hopLimit);
    }
}

void MessageHandler::parseSearchRequest(QVariantMap textVariantMap, Peer currentPeer) {
    QString originName = textVariantMap[sock->DEFAULT_ORIGIN_KEY].toString();
    QString keywords = textVariantMap[sock->DEFAULT_SEARCH_KEY].toString();
    quint32 budget = textVariantMap[sock->DEFAULT_BUDGET_KEY].toUInt();

    //qDebug() << "MessageHandler: origin is " << originName;
    emit(gotNewSearchRequest(originName, keywords, budget));
}

void MessageHandler::parseSearchReply(QVariantMap textVariantMap, Peer currentPeer) {
    QString dest = textVariantMap[sock->DEFAULT_DEST_KEY].toString();
    quint32 hopLimit = (textVariantMap[sock->DEFAULT_HOP_LIMIT_KEY].toInt()) - 1;
    QString originName = textVariantMap[sock->DEFAULT_ORIGIN_KEY].toString();
    QString keywords = textVariantMap[sock->DEFAULT_SEARCH_REPLY_KEY].toString();
    QVariantList matchNames = textVariantMap[sock->DEFAULT_SEARCH_REPLY_NAMES_KEY].toList();
    QVariantList matchIDs = textVariantMap[sock->DEFAULT_SEARCH_REPLY_IDS_KEY].toList();

    qDebug() << "Got search reply from " << originName << " for " << keywords << "of size " << matchNames.size();
    qDebug() << "Search reply is for: " << dest << "first hop is " << routingMap[dest].port;

    if (dest == localhostName) {
        emit(gotNewSearchReply(originName, keywords, matchNames, matchIDs));
    } else {
        if (!checkForward(hopLimit, dest))
            return;

        textVariantMap[sock->DEFAULT_HOP_LIMIT_KEY] = hopLimit;
        sock->sendVariantMap(textVariantMap, routingMap[dest]);
    }
}

void MessageHandler::parse(QByteArray *serializedMessage, QHostAddress sender, quint16 senderPort, QMap<QString, QVector<QString> > messages, QMap<QString, Peer> routingMap) {
    this->messages = messages;
    this->routingMap = routingMap;

    QVariantMap textVariantMap;
    QDataStream *deserializer = new QDataStream(serializedMessage, QIODevice::ReadOnly);
    (*deserializer) >> textVariantMap;    

    Peer currentPeer(sender, senderPort);
    emit handlerAddPeerToList(currentPeer);    

    if (textVariantMap.contains(sock->DEFAULT_SEQ_NO_KEY)) { // Rumor chat or route message
        parseRumorMessage(textVariantMap, currentPeer);
        return;
    }

    if (textVariantMap.contains(sock->DEFAULT_WANT_KEY)) { // Status message
        parseStatusMessage(textVariantMap, currentPeer);
        return;
    }

    if (textVariantMap.contains(sock->DEFAULT_DEST_KEY) && textVariantMap.contains(sock->DEFAULT_TEXT_KEY)) { // Received private message
        parsePrivateMessage(textVariantMap, currentPeer);
        return;
    }

    if (textVariantMap.contains(sock->DEFAULT_DEST_KEY) && textVariantMap.contains(sock->DEFAULT_BLOCK_REQUEST_KEY)) { // Received block request
        parseBlockRequest(textVariantMap, currentPeer);
        return;
    }

    if (textVariantMap.contains(sock->DEFAULT_DEST_KEY) && textVariantMap.contains(sock->DEFAULT_BLOCK_REPLY_KEY)) { // Received block response
        parseBlockResponse(textVariantMap, currentPeer);
        return;
    }

    if (textVariantMap.contains(sock->DEFAULT_SEARCH_KEY)) {
        parseSearchRequest(textVariantMap, currentPeer);
        return;
    }

    if (textVariantMap.contains(sock->DEFAULT_SEARCH_REPLY_KEY)) {
        parseSearchReply(textVariantMap, currentPeer);
        return;
    }

    qDebug() << "Received bad map from: " << sender << " " << senderPort;
    printMap(textVariantMap);
    return;
}

bool MessageHandler::checkForward(quint32 hopLimit, QString dest) {
    if (hopLimit == 0) // Drop
        return false;
    if (!routingMap.contains(dest)) {
        qDebug() << "[Warning]: Received private message, but map entry for its destination is empty!!!";
        return false;
    }

    return true;
}

bool MessageHandler::checkHash(QByteArray data, QByteArray hash) {
    QCA::Hash shaHash("sha256");
    QByteArray blockHash = shaHash.hash(data).toByteArray();
    return (blockHash == hash);
}

void MessageHandler::printMap(QVariantMap map) {
    QVariantMap::iterator it;
    qDebug() << "Printing received variant map\n";
    for (it = map.begin(); it != map.end(); ++it) {
        qDebug() << "    " << it.key() << it.value();
    }

    qDebug() << "\n\n";
}


