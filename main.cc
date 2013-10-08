#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QVariantMap>
#include <QTimer>
#include <QLabel>
#include <QMessageBox>

#include "main.hh"

const int ChatDialog::ANTI_ENTROPY_FREQ = 2000;
const int ChatDialog::ROUTE_MESSAGE_FREQ = 10000;
const quint32 ChatDialog::HOP_LIMIT = 10;
const quint32 ChatDialog::BUDGET_LIMIT = 100;
const int ChatDialog::KEYWORD_SEARCH_FREQ = 1000;
const int ChatDialog::KEYWORD_SEARCH_MAX_RESULTS = 10;

ChatDialog::ChatDialog(bool noForwardFlag)
{
    this->noForwardFlag = noForwardFlag;
    sock = new NetSocket(noForwardFlag);
    if (!sock->bind())
        exit(1);
    connect(sock, SIGNAL(readyRead()), this, SLOT(receiveMessage()));

    srand(time(0));

    localhostName = QHostInfo::localHostName() + "-" + QString::number(rand() % 1000000) + ":" + QString::number(sock->getCurrentPort());    
    localhost = new Peer(QHostAddress::LocalHost, sock->getCurrentPort());    

    messageHandler = new MessageHandler(sock, localhostName);
    connect(messageHandler, SIGNAL(gotNewRumorMessage(Peer, QString, QString, quint32, quint32)),
            this, SLOT(addReceivedMessage(Peer, QString, QString, quint32, quint32)));
    connect(messageHandler, SIGNAL(gotNewPrivateMessage(Peer, QString, QString, quint32, quint32)),
            this, SLOT(addReceivedMessage(Peer, QString, QString, quint32, quint32)));
    connect(messageHandler, SIGNAL(gotNewBlockRequest(QString, quint32, QString, QByteArray)),
            this, SLOT(handleBlockRequest(QString, quint32, QString, QByteArray)));
    connect(messageHandler, SIGNAL(handlerAddPeerToList(Peer)), this, SLOT(addPeerToList(Peer)));
    connect(messageHandler, SIGNAL(gotNewSearchRequest(QString,QString,quint32)),
            this, SLOT(parseSearchRequest(QString, QString, quint32)));
    connect(messageHandler, SIGNAL(gotNewSearchReply(QString,QString,QVariantList,QVariantList)),
            this, SLOT(parseSearchReply(QString,QString,QVariantList,QVariantList)));

    fileManager = new FileManager(sock, localhostName);
    connect(this, SIGNAL(retrieveFileByID(QByteArray, QString, Peer, quint32)),
            fileManager, SLOT(retrieveFile(QByteArray, QString, Peer, quint32)));
    connect(fileManager, SIGNAL(blockReadyForSending(QByteArray, QString, quint32)),
            this, SLOT(sendBlockRequest(QByteArray, QString, quint32)));
    connect(fileManager, SIGNAL(completedTransfer(QByteArray, QString)),
            this, SLOT(transferComplete(QByteArray, QString)));


    connect(messageHandler, SIGNAL(gotNewBlockResponse(QString,QByteArray,QByteArray)),
            fileManager, SLOT(gotNewBlockResponse(QString,QByteArray,QByteArray)));

    setUpUI();

    discoverPeers();

    QTimer *antiEntropyTimer = new QTimer(this);
    connect(antiEntropyTimer, SIGNAL(timeout()), this, SLOT(antiEntropySendStatus()));
    antiEntropyTimer->start(ANTI_ENTROPY_FREQ);

    QTimer *routeMessageTimer = new QTimer(this);
    connect(routeMessageTimer, SIGNAL(timeout()), this, SLOT(sendRouteMessage()));
    routeMessageTimer->start(ROUTE_MESSAGE_FREQ);    

    connect(this, SIGNAL(gotNewMessage(Peer, QString, QString, quint32, quint32)),
            this, SLOT(addReceivedMessage(Peer, QString, QString, quint32, quint32)));

    sendRouteMessage();    
}

void ChatDialog::setUpUI() {
    setWindowTitle("Peerster - " + localhostName);

    textview = new QTextEdit(this);
    textview->setReadOnly(true);

    textline = new MultiLineEdit(this);
    textline->setFocus();
    connect(textline, SIGNAL(returnSignal()),
        this, SLOT(gotReturnPressed()));

    addressLine = new QLineEdit(this);
    addressLabel = new QLabel("Address", this);

    portLine = new QLineEdit(this);
    portLabel = new QLabel("Port", this);

    addPeerButton = new QPushButton("Add peer", this);

    peerview = new QTextEdit(this);
    peerview->setReadOnly(true);
    peerviewLabel = new QLabel("Hosts", this);

    peerNameList = new QListWidget(this);
    connect(peerNameList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(peerClicked(QListWidgetItem*)));
    peerNameListLabel = new QLabel("Peers", this);

    connect(addPeerButton, SIGNAL(clicked()), this, SLOT(addNewPeerFromUI()));

    shareFileButton = new QPushButton("Share file", this);
    connect(shareFileButton, SIGNAL(clicked()), this, SLOT(openFileDialog()));

    fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);

    shareView = new QListWidget(this);
    shareLabel = new QLabel("Shared files", this);    

    pendingView = new QListWidget(this);
    pendingLabel = new QLabel("Pending transfers", this);
    searchBySHA = new QLabel("Search by file SHA256 hash", this);
    shaSearchLine = new QLineEdit(this);
    nodeToAskLabel = new QLabel("Peer to ask", this);
    nodeToAskLine = new QComboBox(this);

    shaSearchButton = new QPushButton("Search!", this);
    connect(shaSearchButton, SIGNAL(clicked()), this, SLOT(searchByShaClicked()));

    downloadsDialog = new DownloadsDialog(this);
    connect(downloadsDialog, SIGNAL(downloadRequest(QString,QByteArray,QString)),
            this, SLOT(downloadRequest(QString,QByteArray,QString)));

    keywordSearchLine = new QLineEdit(this);
    keywordSearchButton = new QPushButton("Search by keyword", this);
    connect(keywordSearchButton, SIGNAL(clicked()), this, SLOT(searchByKeywordClicked()));

    QHBoxLayout *addressLayout = new QHBoxLayout();
    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(addressLine);

    QHBoxLayout *portLayout = new QHBoxLayout();
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portLine);

    QVBoxLayout *peerLayout = new QVBoxLayout();
    peerLayout->addWidget(peerNameListLabel, 1, Qt::AlignCenter);
    peerLayout->addWidget(peerNameList, 15);
    peerLayout->addWidget(peerviewLabel, 1, Qt::AlignCenter);
    peerLayout->addWidget(peerview, 15);
    peerLayout->addLayout(addressLayout, 1);
    peerLayout->addLayout(portLayout, 1);
    peerLayout->addWidget(addPeerButton, 2);    

    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->addWidget(textview, 50);
    textLayout->addWidget(textline, 10);

    QVBoxLayout *shareLayout = new QVBoxLayout();
    shareLayout->addWidget(shareLabel, 1, Qt::AlignCenter);
    shareLayout->addWidget(shareView, 50);
    shareLayout->addWidget(shareFileButton, 10);

    QVBoxLayout *transferSearchLayout = new QVBoxLayout();
    transferSearchLayout->addWidget(pendingLabel, 1, Qt::AlignCenter);
    transferSearchLayout->addWidget(pendingView, 20);
    transferSearchLayout->addWidget(searchBySHA, 1, Qt::AlignCenter);
    transferSearchLayout->addWidget(shaSearchLine, 1);
    transferSearchLayout->addWidget(nodeToAskLabel, 1, Qt::AlignCenter);
    transferSearchLayout->addWidget(nodeToAskLine, 1);
    transferSearchLayout->addWidget(shaSearchButton, 1);

    QVBoxLayout *keywordSearchLayout = new QVBoxLayout();
    keywordSearchLayout->addWidget(keywordSearchLine, 1, Qt::AlignCenter);
    keywordSearchLayout->addWidget(keywordSearchButton, 1, Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addLayout(textLayout, 2);
    layout->addLayout(peerLayout, 2);
    layout->addLayout(transferSearchLayout, 2);
    layout->addLayout(shareLayout, 2);
    layout->addLayout(keywordSearchLayout, 4);

    setLayout(layout);
}

void ChatDialog::downloadRequest(QString fileName, QByteArray shaHash, QString owner) {
    QMessageBox::warning(this, "Success", "Starting download for " + fileName);
    pendingView->addItem(shaHash.toHex());
    emit(retrieveFileByID(shaHash, owner, routingMap[owner], HOP_LIMIT, fileName));
}

void ChatDialog::parseSearchReply(QString originName, QString keywords, QVariantList matchNames, QVariantList matchIDs) {
    // add the stuff to table
    // disconnect timers if I have more than 10 results
    if (keywordSearchResults[keywords] > 10)
        return;

    keywordSearchResults[keywords] += matchNames.size();

    if (keywordSearchResults[keywords] > 10)
        keywordSearchTimer[keywords]->disconnect();

    //TODO: finish
    for (int i = 0; i < matchNames.size(); i++) {
        QString currName = matchNames.at(i).toString();
        QString currID = matchIDs.at(i).toByteArray().toHex();
        downloadsDialog->append(currName, currID, originName);
    }
}

void ChatDialog::parseSearchRequest(QString originName, QString keywords, quint32 budget) {
    QList<QPair<QString, QByteArray> > searchResults = fileManager->searchByKeyword(keywords);
    qDebug() << "In main: got" << searchResults.size() << "results";
    sendKeywordSearchRequest(keywords, budget);
    qDebug() << "Sent requests to neighbours with budget " << budget;
    qDebug() << "ChatDialog: origin is " << originName;

    if (searchResults.size() != 0)
        sock->sendSearchReply(localhostName, originName, HOP_LIMIT, keywords, searchResults, routingMap[originName]);
}

void ChatDialog::sendKeywordSearchRequest(QString keyword, quint32 budget) {
    budget--;

    if (budget == 0)
        return;

    for (int i = 0; i < peerNameList->count(); i++) {
        int peerBudget = budget / peerNameList->count() + (i < budget % peerNameList->count());
        QString currentPeerName = peerNameList->item(i)->text();
        if (peerBudget > 0)
            sock->sendSearchRequest(localhostName, keyword, peerBudget, routingMap[currentPeerName]);
    }
}

void ChatDialog::newKeywordSearchRequest() {
    QObject *signalSender = sender();

    KeywordTimer *kTimer = static_cast<KeywordTimer*>(signalSender);
    QString keyword = kTimer->getKeyword();

    if (keywordSearchBudget[keyword] > BUDGET_LIMIT || keywordSearchResults[keyword] > KEYWORD_SEARCH_MAX_RESULTS) {
        keywordSearchTimer[keyword]->disconnect();
        return;
    }

    sendKeywordSearchRequest(keyword, keywordSearchBudget[keyword]);

    keywordSearchBudget[keyword] = 2 * keywordSearchBudget[keyword];
}

void ChatDialog::searchByKeywordClicked() {
    QString keywords = keywordSearchLine->text();
    qDebug() << keywords;
    if (keywords.isEmpty()) {
        QMessageBox::warning(this, "Error", "Search box empty!");
        return;
    }

    keywordSearchBudget[keywords] = 2;
    keywordSearchResults[keywords] = 0;
    keywordSearchTimer[keywords] = new KeywordTimer(this, keywords);
    connect(keywordSearchTimer[keywords], SIGNAL(timeout()), this, SLOT(newKeywordSearchRequest()));
    keywordSearchTimer[keywords]->start(KEYWORD_SEARCH_FREQ);

    downloadsDialog->showDialog();
}

void ChatDialog::searchByShaClicked() {
    QString peerName = nodeToAskLine->currentText();
    if (!routingMap.contains(peerName)) {
        QMessageBox::warning(this, "Error", "Peer not found!");
        return;
    }

    QString shaHashString = shaSearchLine->text();
    if (shaHashString.isEmpty()) {
        QMessageBox::warning(this, "Error", "SHA box is empty!");
        return;
    }

    pendingView->addItem(shaHashString);
    QByteArray shaHash = QByteArray::fromHex(shaHashString.toAscii());
    emit(retrieveFileByID(shaHash, peerName, routingMap[peerName], HOP_LIMIT));
}

void ChatDialog::transferComplete(QByteArray fileID, QString fileName) {
    QString fileIDString = fileID.toHex();
    qDebug() << "Transfer complete for " << fileIDString;

    QList<QListWidgetItem*> list = pendingView->findItems(fileIDString, Qt::MatchExactly);
    if (list.size() != 1) {
        qDebug() << "UI transfers pending list could not find string " << fileIDString;
        return;
    }

    QListWidgetItem* item = list.at(0);
    pendingView->removeItemWidget(item);
    delete item;

    shareView->addItem(fileName);
    fileManager->addFile(fileName);
}

void ChatDialog::openFileDialog() {
    // TODO: Support directories
    QStringList fileNames;
    fileDialog->show();
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
        fileDialog->hide();
        for (int i = 0; i < fileNames.size(); i++) {
            qDebug() << fileNames.at(i) << "\n";

            shareView->addItem(fileNames.at(i));
            fileManager->addFile(fileNames.at(i));
        }
    }
}

void ChatDialog::createPrivateDialog(QString peerName) {
    if (privateChatMap.contains(peerName)) {
        if (privateChatMap[peerName] == NULL) {
            privateChatMap[peerName] = new PrivateChatDialog(this, peerName, localhostName);
            connect(privateChatMap[peerName], SIGNAL(privateChatSendMessage(Peer, QString, QString, quint32, quint32)),
                    this, SLOT(addReceivedMessage(Peer, QString, QString, quint32, quint32)));
            connect(this, SIGNAL(receivedPrivateMessage(QString, QString)),
                    privateChatMap[peerName], SLOT(addReceivedPrivateMessage(QString, QString)));
        } else {
            privateChatMap[peerName]->show();
        }
    } else {
        privateChatMap[peerName] = new PrivateChatDialog(this, peerName, localhostName);
        connect(privateChatMap[peerName], SIGNAL(privateChatSendMessage(Peer, QString, QString, quint32, quint32)),
                this, SLOT(addReceivedMessage(Peer, QString, QString, quint32, quint32)));
        connect(this, SIGNAL(receivedPrivateMessage(QString, QString)),
                privateChatMap[peerName], SLOT(addReceivedPrivateMessage(QString, QString)));
    }

    privateChatMap[peerName]->show();
    privateChatMap[peerName]->setFocus();

    qDebug() << "Create private dialog for" << peerName;
}

void ChatDialog::peerClicked(QListWidgetItem *item) {
    createPrivateDialog(item->text());
}

void ChatDialog::discoverPeers() {
    QVector<int> peerPorts = sock->getLocalhostPorts();
    int localPort = localhost->port;
    
    for (int p = 0; p < peerPorts.size(); p++) {
        if (abs(peerPorts[p] - localPort) > 1)
            continue;
        Peer currentPeer(QHostAddress::LocalHost, peerPorts[p]);
        addPeerToList(currentPeer);
    }
}

void ChatDialog::addPeerToList(Peer peer) {
    if (!peerList.contains(peer)) {
        peerList.push_back(peer);
        qDebug() << peer.toString();
        peerview->append(peer.toString());
    }
}

void ChatDialog::addNewPeerFromUI() {
    QString address = addressLine->text();
    lastAddedPort = portLine->text();
    QString portS = portLine->text();

    addNewPeerGeneral(address, portS);
}

void ChatDialog::addNewPeerCommandline(QString fullAddress) {
    int splitPos = fullAddress.indexOf(":");
    if (splitPos == -1) {
        if (fullAddress != "-noforward")
            qDebug() << "Wrong format for commandline argument!";
        return;
    }

    QString address = fullAddress.left(splitPos);
    QString portS = fullAddress.mid(splitPos + 1, fullAddress.length() - address.length() - 1);

    addNewPeerGeneral(address, portS);
}

void ChatDialog::addNewPeerGeneral(QString address, QString portS) {
    lastAddedPort = portS;

    QHostAddress hostAddress(address);
    quint16 port = portS.toInt();

    if (port == 0) {
        qDebug() << "Invalid port!";
        return;
    }

    qDebug() << "New peer:" << hostAddress << ":" << port;

    if (hostAddress.isNull()) {
        QHostInfo::lookupHost(address, this, SLOT(lookedUp(QHostInfo)));
    } else {
        Peer newPeer(hostAddress, port);
        addPeerToList(newPeer);
        peerTimers.push_back(new QTimer(this));
        addressLine->clear();
        portLine->clear();
    }
}

void ChatDialog::lookedUp(QHostInfo host) {
    if (host.addresses().isEmpty()) {
        qDebug() << "Invalid address";
        return;
    }

    qDebug() << "New host: " << host.addresses().at(0) << lastAddedPort.toInt();

    Peer newPeer(host.addresses().at(0), lastAddedPort.toInt());
    addPeerToList(newPeer);

    addressLine->clear();
    portLine->clear();
}

void ChatDialog::gotReturnPressed()
{
	// Initially, just echo the string locally.
	// Insert some networking code here...    
    QString message = textline->toPlainText();

    emit(gotNewMessage(*localhost, localhostName, message, messages[localhostName].size() + 1));

	// Clear the textline to get ready for the next input message.
	textline->clear();
}

void ChatDialog::spreadRumor(Peer previous, QString from, QString message, int position) {
    srand(time(0));

    if (message != NULL) { // Chat rumor
        while (1) {
            Peer randomPeer = peerList[rand() % peerList.size()];
            //qDebug() << "Rumormongering: sending message to peer: " << randomPeer.hostAddress << randomPeer.port;
            sock->sendMessage(from, message, position, randomPeer, previous.hostAddress.toIPv4Address(), previous.port);

            if (rand() % 2 == 0)
                return;
        }
    } else { // Route rumor -> send to everybody
        for (int i = 0; i < peerList.size(); i++) {
            Peer currentPeer = peerList[i];
            sock->sendMessage(from, message, position, currentPeer, previous.hostAddress.toIPv4Address(), previous.port);
        }
    }
}

void ChatDialog::sendRouteMessage() {
    emit(gotNewMessage(*localhost, localhostName, NULL, messages[localhostName].size() + 1));
}

void ChatDialog::antiEntropySendStatus() {
    srand(time(0));

    Peer randomPeer = peerList[rand() % peerList.size()];
    sock->sendStatus(randomPeer, messages);    
}

int ChatDialog::addReceivedMessage(Peer senderPeer, QString peerName, QString message, quint32 seqNo, quint32 isDirect) {    
    if (seqNo != sock->SEND_PRIVATE && seqNo != sock->RECEIVE_PRIVATE) { // Gossip message case
        int localSeqNo = messages[peerName].size() + 1;        
        // NEW MESSAGE! PROPAGAAAATE!!!
        if (seqNo == localSeqNo) {
            if (message != NULL) {
                textview->append("[" + peerName + "]: " + message);                
            }

            messages[peerName].push_back(message);

            // Add origin/sender to routing map
            if (!routingMap.contains(peerName)) {
                //if (peerName != localhostName)
                peerNameList->addItem(peerName);
                nodeToAskLine->addItem(peerName);
            }
            routingMap[peerName] = senderPeer;
            printRoutingMap(routingMap);

            // Send status to sender of this message
            sock->sendStatus(senderPeer, messages);

            // Rumormongering
            spreadRumor(senderPeer, peerName, message, messages[peerName].size());
            return 0;
        } else if (seqNo < localSeqNo) {
            if (isDirect) {                
                routingMap[peerName] = senderPeer;
            }
            return 1; // This message has already been received
        } else { // It's bigger so I skipped a few messages; need to retrieve them first
            sock->sendStatus(senderPeer, messages); // send a status map and start gossip with the sender, until I'm up to date
            return -1;
        }
    } else { // Private message case
        // In this case seqNo serves as the type of operation
        if (seqNo == sock->SEND_PRIVATE) { // Sending a private message
            if (!routingMap.contains(peerName)) {
                qDebug() << "!!! TRYING TO SEND MESSAGE TO PEER NOT IN MAP WTF !!! Entering infinite loop";
                //while (1);
            }
            qDebug() << "Private message sending: " << routingMap[peerName].hostAddress << routingMap[peerName].port;

            sock->sendPrivateMessage(localhostName, peerName, message, routingMap[peerName], HOP_LIMIT);
        } else if (seqNo == sock->RECEIVE_PRIVATE) { // Receiving a private message
            qDebug() << "Add received message peer name:" << peerName;
            createPrivateDialog(peerName);
            emit(receivedPrivateMessage(peerName, message));
        } else {
            qDebug() << "seqNo is not SEND_PRIVATE or RECEIVE_PRIVATE for private message! Entering infinite loop";
            while (1);
        }
    }

    return 0;
}

void ChatDialog::receiveMessage() {
    while (sock->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        datagram->resize(sock->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        sock->readDatagram(datagram->data(), datagram->size(), &sender, &senderPort);
        
        messageHandler->parse(datagram, sender, senderPort, messages, routingMap);
    }
}

void ChatDialog::printMap(QVariantMap map, QString hostName) {
    QVariantMap::iterator it;
    qDebug() << "Status map for host " << hostName << ":";
    for (it = map.begin(); it != map.end(); ++it) {
        qDebug() << "    " << it.key() << it.value();
    }
}

void ChatDialog::printRoutingMap(QMap<QString, Peer> map) {
    /*
    QMap<QString, Peer>::iterator it;
    qDebug() << "Routing map for localhost:";
    for (it = map.begin(); it != map.end(); ++it) {
        qDebug() << it.key() << ":" << it.value().hostAddress << it.value().port;
    }
    */
}

void ChatDialog::handleBlockRequest(QString dest, quint32 hopLimit, QString originName, QByteArray requestedBlock) {
    if (!routingMap.contains(dest)) {
        qDebug() << dest << "is not in my routing map!";
        return;
    }

    qDebug() << "Entered handleBlockRequest";

    QByteArray data = fileManager->getBlockByHash(requestedBlock);
    if (data.isEmpty()) {
        qDebug() << "Requested block not found!";
        return;
    }

    /*
    QTime dieTime= QTime::currentTime().addSecs(1);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    */

    sock->sendBlockReply(localhostName, originName, requestedBlock, data, routingMap[originName], hopLimit);
}

void ChatDialog::sendBlockRequest(QByteArray block, QString originName, quint32 blockID) {
    //qDebug() << "Sending block request for block " << blockID;
    if (!routingMap.contains(originName)) {
        qDebug() << originName << "is not in my routing map!";
        return;
    }

    sock->sendBlockRequest(localhostName, originName, block, routingMap[originName], HOP_LIMIT);
}

int main(int argc, char **argv)
{
    // Initialize QCA
    QCA::Initializer qcainit;

    // Initialize Qt toolkit
	QApplication app(argc,argv);    

    // Show initial chat dialog window

    bool noForwardFlag = 0;
    QStringList arguments = QCoreApplication::arguments();
    for (int i = 1; i < arguments.size(); i++) {
        if (arguments.at(i) == "-noforward")
            noForwardFlag = 1;
    }

    ChatDialog *dialog = new ChatDialog(noForwardFlag);
    dialog->show();

    if (arguments.size() > 1) {
        for (int i = 1; i < arguments.size(); i++) {
            dialog->addNewPeerCommandline(arguments.at(i));
        }
    }

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}

