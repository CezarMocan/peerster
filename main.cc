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

#include "main.hh"

const int ChatDialog::ANTI_ENTROPY_FREQ = 5000;
const int ChatDialog::ROUTE_MESSAGE_FREQ = 60000;

bool ReturnKeyFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            emit returnKeyPressedSignal();
            return true;
        }
        else {
            return QObject::eventFilter(obj, event);
        }
    } else {
        return QObject::eventFilter(obj, event);
    }
}

void MultiLineEdit::returnKeyPressedSlot() {
    emit returnSignal();
}

ChatDialog::ChatDialog()
{
    sock = new NetSocket();
    if (!sock->bind())
        exit(1);

    connect(sock, SIGNAL(readyRead()), this, SLOT(receiveMessage()));

    srand(time(0));

    localhostName = QHostInfo::localHostName() + "-" + QString::number(rand() % 1000000) + ":" + QString::number(sock->getCurrentPort());    
    localhost = new Peer(QHostAddress::LocalHost, sock->getCurrentPort());    

	setWindowTitle("Peerster - " + localhostName);

	// Read-only text box where we display messages from everyone.
	// This widget expands both horizontally and vertically.
	textview = new QTextEdit(this);
	textview->setReadOnly(true);

	// Small text-entry box the user can enter messages.
	// This widget normally expands only horizontally,
	// leaving extra vertical space for the textview widget.
	//
	// You might change this into a read/write QTextEdit,
	// so that the user can easily enter multi-line messages.
	
    returnKeyFilter = new ReturnKeyFilter();
    
    textline = new MultiLineEdit(this);
    textline->installEventFilter(returnKeyFilter);

    connect(returnKeyFilter, SIGNAL(returnKeyPressedSignal()),
        textline, SLOT(returnKeyPressedSlot()));


    addressLine = new QLineEdit(this);
    addressLabel = new QLabel("Address", this);

    portLine = new QLineEdit(this);
    portLabel = new QLabel("Port", this);

    addPeerButton = new QPushButton("Add peer", this);

    peerview = new QTextEdit(this);
    peerview->setReadOnly(true);

    connect(addPeerButton, SIGNAL(clicked()), this, SLOT(addNewPeerFromUI()));

	// Lay out the widgets to appear in the main window.
	// For Qt widget and layout concepts see:
	// http://doc.qt.nokia.com/4.7-snapshot/widgets-and-layouts.html

    QHBoxLayout *addressLayout = new QHBoxLayout();
    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(addressLine);
    //layout->addItem(addressLayout);

    QHBoxLayout *portLayout = new QHBoxLayout();
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portLine);
    //layout->addItem(portLayout);

    QVBoxLayout *peerLayout = new QVBoxLayout();
    peerLayout->addWidget(peerview);
    peerLayout->addLayout(addressLayout, 0);
    peerLayout->addLayout(portLayout, 1);
    peerLayout->addWidget(addPeerButton);

    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->addWidget(textview);
    textLayout->addWidget(textline);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addLayout(textLayout, 0);
    layout->addLayout(peerLayout, 1);
    layout->setStretchFactor(textLayout, 2);
    layout->setStretchFactor(peerLayout, 1);

    //layout->addWidget(addPeerButton);
	setLayout(layout);

    // Set focus on the text box before, so the user doesn't have to do it
    textline->setFocus();

	// Register a callback on the textline's returnPressed signal
	// so that we can send the message entered by the user.
	connect(textline, SIGNAL(returnSignal()),
		this, SLOT(gotReturnPressed()));

    discoverPeers();

    QTimer *antiEntropyTimer = new QTimer(this);
    connect(antiEntropyTimer, SIGNAL(timeout()), this, SLOT(antiEntropySendStatus()));
    antiEntropyTimer->start(ANTI_ENTROPY_FREQ);

    QTimer *routeMessageTimer = new QTimer(this);
    connect(routeMessageTimer, SIGNAL(timeout()), this, SLOT(sendRouteMessage()));
    routeMessageTimer->start(ROUTE_MESSAGE_FREQ);

    connect(this, SIGNAL(gotNewMessage(Peer, QString, QString, quint32)), this, SLOT(addReceivedMessage(Peer, QString, QString, quint32)));

    sendRouteMessage();
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

void ChatDialog::spreadRumor(QString from, QString message, int position) {
    srand(time(0));

    while (1) {
        Peer randomPeer = peerList[rand() % peerList.size()];
        //qDebug() << "Rumormongering: sending message to peer: " << randomPeer.hostAddress << randomPeer.port;
        sock->sendMessage(from, message, position, randomPeer);

        if (rand() % 2 == 0)
            return;
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

int ChatDialog::addReceivedMessage(Peer senderPeer, QString peerName, QString message, quint32 seqNo) {
    int localSeqNo = messages[peerName].size() + 1;        

     qDebug() << localhostName << " Received message from: " << senderPeer.hostAddress << ":" << senderPeer.port << "message = " << message << seqNo << localSeqNo;

    // NEW MESSAGE! PROPAGAAAATE!!!
    if (seqNo == localSeqNo) {
        if (message != NULL) {
            textview->append("[" + peerName + "]: " + message);
        }

        messages[peerName].push_back(message);

        // Add origin/sender to routing map
        qDebug() << "Add received message: " << peerName << senderPeer.hostAddress << ":" << senderPeer.port;
        routingMap[peerName] = senderPeer;
        printRoutingMap(routingMap);

        // Send status to sender of this message
        sock->sendStatus(senderPeer, messages);

        // Rumormongering
        spreadRumor(peerName, message, messages[peerName].size());
        return 0;
    } else if (seqNo < localSeqNo) {
        return 1; // This message has already been received
    } else { // It's bigger so I skipped a few messages; need to retrieve them first
        sock->sendStatus(senderPeer, messages); // send a status map and start gossip with the sender, until I'm up to date
        return -1;     
    }
}

// Returns 0 for Message, 1 for Status, -1 for error
int ChatDialog::parseMessage(QByteArray *serializedMessage, QHostAddress sender, quint16 senderPort) {
    QVariantMap textVariantMap;
    QDataStream *deserializer = new QDataStream(serializedMessage, QIODevice::ReadOnly);
    (*deserializer) >> textVariantMap;

    if (textVariantMap.contains(sock->DEFAULT_ORIGIN_KEY)) {
        QString receivedText = NULL;
        if (textVariantMap.contains(sock->DEFAULT_TEXT_KEY))
            receivedText = textVariantMap[sock->DEFAULT_TEXT_KEY].toString();

        QString originName = textVariantMap[sock->DEFAULT_ORIGIN_KEY].toString();
        quint32 seqNo = textVariantMap[sock->DEFAULT_SEQ_NO_KEY].toUInt();

        Peer currentPeer(sender, senderPort);
        addPeerToList(currentPeer);

        emit(gotNewMessage(currentPeer, originName, receivedText, seqNo)); 

        return 0;
    }

    if (textVariantMap.contains(sock->DEFAULT_WANT_KEY)) {
        // parse want map send stuff to the other machine
        QVariantMap wantMap = textVariantMap[sock->DEFAULT_WANT_KEY].toMap();
        QVariantMap::iterator it;

        // Perform 2 iterations; in first one just check if I have anything new, send it if I do and return
        // in the second one (which I get to only after I send everything i have that my peer doesn't) I send my status map
        // if I find anything that he has and I don't;
        // by doing this the chances of an infinite loop of status messages between me and my peer should be less
        // actually 0 if he does the same, but I can't rely on that

        Peer currentPeer(sender, senderPort);
        addPeerToList(currentPeer);
        //qDebug() << "Received status message from: " << sender << senderPort;

        for (it = wantMap.begin(); it != wantMap.end(); ++it) {
            QString gossipAboutName = it.key();
            int size = it.value().toInt();
            //qDebug() << gossipAboutName;

            if (!messages.contains(gossipAboutName)) {
                qDebug() << "Pula de castor";
                continue;
            }
            //qDebug() << messages[gossipAboutName].size() << size;
            if (messages[gossipAboutName].size() + 1 > size) { // I have more info than he does from this host, I'll send him message and quit
                sock->sendMessage(gossipAboutName, messages[gossipAboutName][size - 1], size, currentPeer);
                return 0;
            }
        }

        QMap<QString, QVector<QString> >::iterator it2;
        for (it2 = messages.begin(); it2 != messages.end(); ++it2) {
            if (messages[it2.key()].size() == 0)
                continue;
            if (!wantMap.contains(it2.key())) {                
                sock->sendMessage(it2.key(), messages[it2.key()][0], 1, currentPeer);
                return 0;
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
                return 0;
            }       
        }

        return 0;
    }

    qDebug() << "Received bad map from: " << sender << " " << senderPort;
    return -1;
}

void ChatDialog::receiveMessage() {
    while (sock->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        datagram->resize(sock->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        sock->readDatagram(datagram->data(), datagram->size(), &sender, &senderPort);
        
        parseMessage(datagram, sender, senderPort);
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
    QMap<QString, Peer>::iterator it;
    qDebug() << "Routing map for localhost:";
    for (it = map.begin(); it != map.end(); ++it) {
        qDebug() << it.key() << ":" << it.value().hostAddress << it.value().port;
    }
}

int main(int argc, char **argv)
{
	// Initialize Qt toolkit
	QApplication app(argc,argv);

    // Show initial chat dialog window
    ChatDialog *dialog = new ChatDialog();
    dialog->show();

    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() > 1) {
        for (int i = 1; i < arguments.size(); i++) {
            dialog->addNewPeerCommandline(arguments.at(i));
        }
    }

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}

