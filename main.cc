
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

const QString ChatDialog::DEFAULT_TEXT_KEY = QString("ChatText");
const QString ChatDialog::DEFAULT_ORIGIN_KEY = QString("Origin");
const QString ChatDialog::DEFAULT_SEQ_NO_KEY = QString("SeqNo");
const QString ChatDialog::DEFAULT_WANT_KEY = QString("Want");
const int ChatDialog::ANTI_ENTROPY_TIMER = 1000;

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

    localhostName = "cm795:" + QString::number(rand() % 10000) + ":" + QString::number(sock->getCurrentPort());
    //localhostName = QString::number(rand()) + ":" + QString::number(sock->getCurrentPort());
    localhost = new Peer(QHostAddress::LocalHost, sock->getCurrentPort());

    discoverPeers();

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
    addressLabel = new QLabel("Host address", this);

    portLine = new QLineEdit(this);
    portLabel = new QLabel("Host port", this);

    addPeerButton = new QPushButton("Add peer", this);

    connect(addPeerButton, SIGNAL(clicked()), this, SLOT(addNewPeerFromUI()));

	// Lay out the widgets to appear in the main window.
	// For Qt widget and layout concepts see:
	// http://doc.qt.nokia.com/4.7-snapshot/widgets-and-layouts.html
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(textview);
	layout->addWidget(textline);

    QHBoxLayout *addressLayout = new QHBoxLayout();
    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(addressLine);
    layout->addItem(addressLayout);

    QHBoxLayout *portLayout = new QHBoxLayout();
    portLayout->addWidget(portLabel);
    portLayout->addWidget(portLine);
    layout->addItem(portLayout);

    layout->addWidget(addPeerButton);
	setLayout(layout);

    // Set focus on the text box before, so the user doesn't have to do it
    textline->setFocus();

	// Register a callback on the textline's returnPressed signal
	// so that we can send the message entered by the user.
	connect(textline, SIGNAL(returnSignal()),
		this, SLOT(gotReturnPressed()));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(antiEntropySendStatus()));
    timer->start(ANTI_ENTROPY_TIMER);

    connect(this, SIGNAL(gotNewMessage(Peer, QString, QString, quint32)), this, SLOT(addReceivedMessage(Peer, QString, QString, quint32)));
}

void ChatDialog::discoverPeers() {
    QVector<int> peerPorts = sock->getLocalhostPorts();
    int localPort = localhost->port;

    for (int p = 0; p < peerPorts.size(); p++) { 
        Peer currentPeer(QHostAddress::LocalHost, peerPorts[p]);
        peerList.push_back(currentPeer);
    }

//    Peer michael(QHostAddress("128.36.232.49"), 37180);
//    peerList.push_back(michael);
}

void ChatDialog::addNewPeerFromUI() {
    QString address = addressLine->text();
    QString portS = portLine->text();

    QHostAddress hostAddress(address);
    qDebug() << "Host address: " << hostAddress;


    quint16 port = portS.toInt();
    if (port == 0) {
        qDebug() << "Invalid port!";
        return;
    }

    if (hostAddress.isNull()) {
        QHostInfo::lookupHost(address, this, SLOT(lookedUp(QHostInfo)));       
    } else {
        Peer newPeer(hostAddress, port);
        peerList.push_back(newPeer);
        addressLine->clear();
        portLine->clear();
    }
    
}


void ChatDialog::lookedUp(QHostInfo host) {
    if (host.addresses().isEmpty()) {
        qDebug() << "Invalid address";
        return;
    }

    qDebug() << "New host: " << host.addresses().at(0) << portLine->text().toInt();

    Peer newPeer(host.addresses().at(0), portLine->text().toInt());
    peerList.push_back(newPeer);

    addressLine->clear();
    portLine->clear();
}


QVariantMap ChatDialog::serializeMessage(QString fromName, QString text, int position) {
    // Create the QVariantMap containing the message
    QVariantMap textVariantMap;
    textVariantMap.clear();
    textVariantMap.insert(DEFAULT_TEXT_KEY, QVariant(text));
    textVariantMap.insert(DEFAULT_ORIGIN_KEY, QVariant(fromName));
    textVariantMap.insert(DEFAULT_SEQ_NO_KEY, QVariant(position));

    return textVariantMap;
}

QByteArray ChatDialog::serializeVariantMap(QVariantMap map) {
    QByteArray *serializedMessage = new QByteArray();

    QDataStream *serializer = new QDataStream(serializedMessage, QIODevice::WriteOnly);
    (*serializer) << map;

    return (*serializedMessage);
}

// Sends message to all peer list
void ChatDialog::sendMessage(QString from, QString message, int position) {
    QByteArray serializedMessage = serializeVariantMap(serializeMessage(from, message, position));
    sock->writeDatagramPeerList(&serializedMessage, peerList);
}

// Sends message only to specified peer
void ChatDialog::sendMessage(QString from, QString message, int position, Peer to) {
    QByteArray serializedMessage = serializeVariantMap(serializeMessage(from, message, position));
    sock->writeDatagramSinglePeer(&serializedMessage, to);
}

// Sends status message to specified peer
void ChatDialog::sendMessage(QVariantMap status, Peer to) {
    QByteArray serializedMessage = serializeVariantMap(status);
    sock->writeDatagramSinglePeer(&serializedMessage, to);
}

void ChatDialog::gotReturnPressed()
{
	// Initially, just echo the string locally.
	// Insert some networking code here...
    QString message = textline->toPlainText();

//	textview->append("[" + localhostName + "]: " + message); 

    //messages[localhostName].push_back(message);

    emit(gotNewMessage(*localhost, localhostName, message, messages[localhostName].size() + 1));

    //addReceivedMessage(*localhost, localhostName, message, messages[localhostName].size() + 1);
    //sendMessage(localhostName, message, messages[localhostName].size());

	// Clear the textline to get ready for the next input message.
	textline->clear();
}

void ChatDialog::spreadRumor(QString from, QString message, int position) {
    srand(time(0));

    while (1) {
        Peer randomPeer = peerList[rand() % peerList.size()];
        // qDebug() << "Rumormongering: sending message to peer: " << randomPeer.hostAddress << randomPeer.port;
        sendMessage(from, message, position, randomPeer);

        if (rand() % 2 == 0)
            return;
    }

//    TODO: Implement the timer functionality here
//    QTimer timer = new QTimer(this);
//    timer->start(2000);
//    connect(timer, SIGNAL(timeout()), this, SLOT(spreadRumor

}

void ChatDialog::sendStatus(Peer from) {
    QVariantMap status = createStatusMap(messages);
    // qDebug() << localhostName << ":Sending status to " << from.hostAddress << ":" << from.port;
    sendMessage(status, from);
}

void ChatDialog::antiEntropySendStatus() {
    srand(time(0));

    Peer randomPeer = peerList[rand() % peerList.size()];
    sendStatus(randomPeer);

//    qDebug() << localhostName << " Anti-entropy: sent status to " << randomPeer.hostAddress << randomPeer.port; 
}

int ChatDialog::addReceivedMessage(Peer senderPeer, QString peerName, QString message, quint32 seqNo) {
    int localSeqNo = messages[peerName].size() + 1; 

    // qDebug() << localhostName << " Received message from: " << senderPeer.hostAddress << ":" << senderPeer.port << "message = " << message << seqNo << localSeqNo;

    // NEW MESSAGE! PROPAGAAAATE!!!
    if (seqNo == localSeqNo) {
        messages[peerName].push_back(message);
        textview->append("[" + peerName + "]: " + message);

        // Send status to sender of this message
        sendStatus(senderPeer);
        spreadRumor(peerName, message, messages[peerName].size());
        return 0;
    } else if (seqNo < localSeqNo) {
        return 1; // This message has already been received
    } else { // It's bigger so I skipped a few messages; need to retrieve them first
        sendStatus(senderPeer); // send a status map and start gossip with the sender, until I'm up to date
        return -1;     
    }
}

// Returns 0 for Message, 1 for Status, -1 for error
int ChatDialog::parseMessage(QByteArray *serializedMessage, QHostAddress sender, quint16 senderPort) {
    QVariantMap textVariantMap;
    QDataStream *deserializer = new QDataStream(serializedMessage, QIODevice::ReadOnly);
    (*deserializer) >> textVariantMap;

    if (textVariantMap.contains(DEFAULT_TEXT_KEY)) {
        QString receivedText = textVariantMap[DEFAULT_TEXT_KEY].toString();
        QString originName = textVariantMap[DEFAULT_ORIGIN_KEY].toString();
        quint32 seqNo = textVariantMap[DEFAULT_SEQ_NO_KEY].toUInt();

        Peer currentPeer(sender, senderPort);
        if (!peerList.contains(currentPeer))
            peerList.push_back(currentPeer);

        emit(gotNewMessage(currentPeer, originName, receivedText, seqNo)); 

        return 0;
    }

    if (textVariantMap.contains(DEFAULT_WANT_KEY)) {
        // parse want map send stuff to the other machine
        QVariantMap wantMap = textVariantMap[DEFAULT_WANT_KEY].toMap(); 
        QVariantMap::iterator it;

        // Perform 2 iterations; in first one just check if I have anything new, send it if I do and return
        // in the second one (which I get to only after I send everything i have that my peer doesn't) I send my status map
        // if I find anything that he has and I don't;
        // by doing this the chances of an infinite loop of status messages between me and my peer should be less
        // actually 0 if he does the same, but I can't rely on that

        Peer currentPeer(sender, senderPort);
        if (!peerList.contains(currentPeer))
            peerList.push_back(currentPeer);

        // qDebug() << "Received status message from: " << sender << senderPort;

        for (it = wantMap.begin(); it != wantMap.end(); ++it) {
            QString gossipAboutName = it.key();
            int size = it.value().toInt();

            if (messages[gossipAboutName].size() + 1 > size) { // I have more info than he does from this host, I'll send him message and quit
                sendMessage(gossipAboutName, messages[gossipAboutName][size - 1], size, currentPeer);
                return 0;
            }
        }

        QMap<QString, QVector<QString> >::iterator it2;
        for (it2 = messages.begin(); it2 != messages.end(); ++it2) {
            if (!wantMap.contains(it2.key()))
                sendMessage(it2.key(), messages[it2.key()][0], 1, currentPeer);
        }

        for (it = wantMap.begin(); it != wantMap.end(); ++it) {
            QString gossipAboutName = it.key();
            int size = it.value().toInt();

            if (messages[gossipAboutName].size() + 1 < size) { // Peer has more info than I do, I'll send dat bitch a status map
                sendStatus(currentPeer);
                return 0;
            }       
        }

        return 2; // this means that my map and my peer's map are equal so I can flip the coin
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

void ChatDialog::receivedStatusFromPeer() { // create a variable in the class with the peer that i'm communicating with now

/*
    while (sock->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();

        datagram->resize(sock->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        sock->readDatagram(datagram->data(), datagram->size(), &sender, &senderPort);

        int type = parseMessage(datagram, sender, senderPort);

        if (type == 1 && rumorPeer->hostAddress == sender && rumorPeer->port == senderPort) {
            newStatusFlag = 1;
            // exchange has already been made in parseMessage, here I just need to know if I received a status response back
            // in order to know what's next in the rumormongering (
        }
    }
*/    
}

void ChatDialog::printMap(QVariantMap map, QString hostName) {
    QVariantMap::iterator it;
    qDebug() << "Status map for host " << hostName << ":";
    for (it = map.begin(); it != map.end(); ++it) {
        qDebug() << "    " << it.key() << it.value();
    }
}

QVariantMap ChatDialog::createStatusMap(QMap<QString, QVector<QString> > messages) {
    QVariantMap statusMap;
    QMap<QString, QVector<QString> >::iterator it;
    for (it = messages.begin(); it != messages.end(); ++it) {
        statusMap.insert(it.key(), it.value().size() + 1);
    }

    QVariantMap returnMap;
    returnMap.insert("Want", statusMap);

    //printMap(statusMap, localhostName);

    return returnMap;
}


int main(int argc, char **argv)
{
	// Initialize Qt toolkit
	QApplication app(argc,argv);

    // Show initial chat dialog window
    ChatDialog *dialog = new ChatDialog();
    dialog->show();


	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}

