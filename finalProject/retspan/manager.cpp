#include "manager.h"
#include "file.h"

Manager::Manager(QObject *parent) : QObject(parent) {
    chordManager = new NetworkManager();
    if (!chordManager->bind()) {
        qDebug() << "Error in binding!";
        exit(0);
    }

    connect(chordManager, SIGNAL(receivedKeywordUpdate(QString,QString,QString)), this, SLOT(receivedKeywordUpdate(QString,QString,QString)));
    connect(chordManager, SIGNAL(receivedKeywordQuery(Node,QString)), this, SLOT(receivedKeywordQuery(Node,QString)));
    connect(chordManager, SIGNAL(receivedKeywordReply(QString,QVariantList,QVariantList)), this, SLOT(receivedKeywordReply(QString,QVariantList,QVariantList)));

    localhost = chordManager->getLocalhost();
    localNode = new ChordNode(chordManager, localhost);
    connect(localNode, SIGNAL(receivedReplyFromChord(QString,Node)), this, SLOT(receivedReplyFromChord(QString, Node)));

    kvs = new KeyValueStore();

    mainWindow = new MainWindow();
    mainWindow->labelNodeID->setText(localhost->getID());
    mainWindow->show();

    connect(mainWindow->connectButton, SIGNAL(clicked()), this, SLOT(connectButtonClicked()));
    connect(mainWindow->buttonSearch, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
    connect(mainWindow, SIGNAL(filesOpened(QStringList)), this, SLOT(filesOpened(QStringList)));
    connect(mainWindow->tableWidgetSearchResults, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(gotDownloadRequest(int,int)));
    connect(this, SIGNAL(keywordSearchReturned(QVariantList,QVariantList)), mainWindow, SLOT(keywordSearchReturned(QVariantList,QVariantList)));

    connect(this, SIGNAL(connectButtonPushed(Node)), localNode, SLOT(connectButtonPushed(Node)));
    connect(localNode, SIGNAL(stateUpdateReady()), mainWindow, SLOT(stateUpdateReady()));
    connect(localNode, SIGNAL(stateUpdateUpdatingOthers()), mainWindow, SLOT(stateUpdateUpdatingOthers()));
    connect(localNode, SIGNAL(updatedFingerTable(QVector<FingerEntry>)), mainWindow, SLOT(updatedFingerTable(QVector<FingerEntry>)));
    connect(localNode, SIGNAL(updatedPredecessor(Node)), mainWindow, SLOT(updatedPredecessor(Node)));

    fileManager = new FileManager(chordManager, kvs);
}

void Manager::receivedReplyFromChord(QString key, Node node) {    
    if (pendingQueries.find(key) != pendingQueries.end()) {
        QPair<QString, QPair<QString, QString> > value = pendingQueries.value(key);
        QString keyword = pendingQueries.value(key).first;
        QString fileID = pendingQueries.value(key).second.first;
        QString fileName = pendingQueries.value(key).second.second;
        QByteArray datagram = Util::serializeVariantMap(Util::createKeywordUpdate(keyword, fileID, fileName));
        chordManager->sendData(node, datagram);
        //qDebug() << "Received reply from chord for: " << keyword << " " << fileName;
        //qDebug() << "Sent keyword update to " << node.toString();
        pendingQueries.remove(key, value);
        //qDebug() << "Removed " << key << " from pendingQueries. Count now is " << pendingQueries.count(key) << "\n";
    }

    if (pendingKeywordQueries.find(key) != pendingKeywordQueries.end()) {
        QString keyword = pendingKeywordQueries.value(key);
        QByteArray datagram = Util::serializeVariantMap(Util::createKeywordQuery(keyword));
        chordManager->sendData(node, datagram);
        pendingKeywordQueries.remove(key, keyword);
        pendingKeywordResponses.insert(keyword, key);
    }

    if (pendingDownloadRequest.find(key) != pendingDownloadRequest.end()) {
        QString fileName = pendingDownloadRequest.value(key);
        fileManager->downloadFile(key, node, fileName, false);
        pendingDownloadRequest.remove(key, fileName);
    }

    if (pendingFileQueries.find(key) != pendingFileQueries.end()) {
        fileManager->uploadFile(key, node);
        pendingFileQueries.remove(key);
    }

    //qDebug() << "Key " << key << " not found in any maps!";
}

void Manager::filesOpened(QStringList fileNames) {
    for (int i = 0; i < fileNames.size(); i++) {
        File file(fileNames[i]);

        kvs->addFile(file.fileID, file.contents);
        qDebug() << "Added " << file.fileID << " to kvs. Size is " << kvs->getFile(file.fileID).size();
        pendingFileQueries.insert(file.fileID, true);
        localNode->chordQuery(file.fileID);
        // Add file to KVS on the node where it was uploaded and then remove from KVS when it's transferred to its owner

        for (int j = 0; j < file.keywords.size(); j++) {
            QString keywordID = file.keywordsID[j];

            QPair<QString, QString> filePair(file.fileID, file.nameWithExtension);
            QPair<QString, QPair<QString, QString> > completePair(file.keywords[j], filePair);            
            pendingQueries.insert(keywordID, completePair);            
            localNode->chordQuery(keywordID);
        }
    }
}

void Manager::receivedKeywordQuery(Node from, QString keyword) {
    QList<QPair<QString, QString> > results = kvs->keywordLookup(keyword);
    QByteArray datagram = Util::serializeVariantMap(Util::createKeywordReply(keyword, results));
    chordManager->sendData(from, datagram);
}

void Manager::receivedKeywordReply(QString keyword, QVariantList ids, QVariantList names) {
    if (pendingKeywordResponses.find(keyword) != pendingKeywordResponses.end()) {
        //qDebug() << "Received keyword reply for keyword " << keyword << " of size " << names.size();
        QString value = pendingKeywordResponses.value(keyword);
        emit(keywordSearchReturned(ids, names));
        pendingKeywordResponses.remove(keyword, value);
    } else {
        qDebug() << "Received keyword reply for unwanted keyword NOOOOOOO!!!";
    }
}

void Manager::receivedKeywordUpdate(QString keyword, QString fileID, QString fileName) {
    //qDebug() << "Keyword update: " << keyword << fileID << fileName;
    kvs->updateKeywordList(keyword, fileID, fileName);
}

void Manager::searchButtonClicked() {
    QString keyword = mainWindow->lineEditSearch->text();
    QString keywordHash = Util::hashName(Util::normalizeKeyword(keyword));
    pendingKeywordQueries.insert(keywordHash, keyword);
    localNode->chordQuery(keywordHash);
}

void Manager::connectButtonClicked() {
    qDebug() << "Button pressed!";
    QHostAddress address(mainWindow->lineEditAddress->text());
    quint16 port = mainWindow->lineEditPort->text().toUInt();
    Node neighbour(address, port);
    emit(connectButtonPushed(neighbour));
}

void Manager::gotDownloadRequest(int row, int column) {
    QString fileName = mainWindow->tableWidgetSearchResults->item(row, 0)->text();
    QString fileID = mainWindow->tableWidgetSearchResults->item(row, 1)->text();
    qDebug() << "Got download request for " << fileName << " with ID " << fileID;
    pendingDownloadRequest.insert(fileID, fileName);
    localNode->chordQuery(fileID);
}
