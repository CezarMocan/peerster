
#include <unistd.h>
#include <stdio.h>

#include <QVBoxLayout>
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QVariantMap>

#include "main.hh"

const QString ChatDialog::DEFAULT_TEXT_KEY = QString("ChatText");
const QString ChatDialog::DEFAULT_ORIGIN_KEY = QString("Origin");
const QString ChatDialog::DEFAULT_SEQ_NO_KEY = QString("SeqNo");
const QString ChatDialog::DEFAULT_WANT_KEY = QString("Want");

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

    localStringName = QString::number(rand()) + ":" + QString::number(sock->getCurrentPort());
    localhost = new Peer(QHostAddress::LocalHost, sock->getCurrentPort());
    nameToPeerMap[localStringName] = *localhost;
    peerToNameMap[*localhost] = localStringName;

    discoverPeers();

	setWindowTitle("Peerster");

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

	// Lay out the widgets to appear in the main window.
	// For Qt widget and layout concepts see:
	// http://doc.qt.nokia.com/4.7-snapshot/widgets-and-layouts.html
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(textview);
	layout->addWidget(textline);
	setLayout(layout);

    // Set focus on the text box before, so the user doesn't have to do it
    textline->setFocus();

	// Register a callback on the textline's returnPressed signal
	// so that we can send the message entered by the user.
	connect(textline, SIGNAL(returnSignal()),
		this, SLOT(gotReturnPressed()));
}

void ChatDialog::discoverPeers() {
    QVector<int> peerPorts = sock->getLocalhostPorts();

    for (int p = 0; p < peerPorts.size(); p++) { 
        Peer currentPeer(QHostAddress::LocalHost, peerPorts[p]);
        peerList.push_back(currentPeer);
    }
}

QVariantMap ChatDialog::serializeMessage(Peer from, QString text, int position) {
    // Create the QVariantMap containing the message
    QVariantMap textVariantMap;
    textVariantMap.clear();
    textVariantMap.insert(DEFAULT_TEXT_KEY, QVariant(text));
    textVariantMap.insert(DEFAULT_ORIGIN_KEY, QVariant(peerToNameMap[from]));
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
void ChatDialog::sendMessage(Peer from, QString message, int position) {
    QByteArray serializedMessage = serializeVariantMap(serializeMessage(from, message, position));
    sock->writeDatagramPeerList(&serializedMessage, peerList);
}

// Sends message only to specified peer
void ChatDialog::sendMessage(Peer from, QString message, int position, Peer to) {
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

	textview->append(message); 

    messages[*localhost].push_back(message);

    sendMessage(*localhost, message, messages[*localhost].size());

	// Clear the textline to get ready for the next input message.
	textline->clear();
}

void ChatDialog::spreadRumor(Peer from, QString message, int position) {
    srand(time(0));

    while (1) {
        Peer randomPeer = peerList[rand() % peerList.size()];

        sendMessage(from, message, position, randomPeer);
        if (rand() % 2 == 0)
            break;
        //QTimer::singleShot(2000, this, SLOT(receivedStatusFromPeer()));
    }
}

void ChatDialog::sendStatus(Peer from) {
    QVariantMap status = createStatusMap(messages);
    qDebug() << peerToNameMap[*localhost] << ":Sending status to " << peerToNameMap[from] << ":" << from.port;
    sendMessage(status, from);
}

int ChatDialog::addReceivedMessage(Peer senderPeer, QString message, quint32 seqNo) {
    int localSeqNo = messages[senderPeer].size() + 1; 

    qDebug() << peerToNameMap[*localhost] << " Received message from: " << senderPeer.hostAddress << ":" << senderPeer.port << "message = " << message << seqNo << localSeqNo;

    // NEW MESSAGE! PROPAGAAAATE!!!
    if (seqNo == localSeqNo) {
        messages[senderPeer].push_back(message);
        textview->append(message);

        // Send status to sender of this message
        sendStatus(senderPeer);
//        spreadRumor(senderPeer, message, messages[senderPeer].size());
        return 0;
    } else if (seqNo < localSeqNo) {
        return 1; // This message has already been received
    } else { // It's bigger so I skipped a few messages; need to retrieve them first
        sendStatus(senderPeer); // send a status map and start gossip with the sender, until I'm up to date
        return -1;     
    }
}

Peer ChatDialog::peerLookupByAddress(QHostAddress address, quint16 port) {
    QMap<QString, Peer>::iterator it;
    for (it = nameToPeerMap.begin(); it != nameToPeerMap.end(); ++it) {
        if (it.value().hostAddress == address && it.value().port == port)
            return it.value();
    }

    Peer dummy;
    return dummy;
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

        nameToPeerMap[originName] = currentPeer;
        peerToNameMap[currentPeer] = originName;

        if (!peerList.contains(currentPeer))
            peerList.push_back(currentPeer);

        int status = addReceivedMessage(currentPeer, receivedText, seqNo);

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

        Peer currentPeer = peerLookupByAddress(sender, senderPort);
        if (currentPeer.port == 0 ) {
            qDebug() << "Received status message from unknown peer... ignoring";
            return -5;
        }

        for (it = wantMap.begin(); it != wantMap.end(); ++it) {
            QString gossipAboutName = it.key();
            int size = it.value().toInt();

            Peer gossipAboutPeer = nameToPeerMap[gossipAboutName];

            if (messages[gossipAboutPeer].size() + 1 > size) { // I have more info than he does from this host, I'll send him message and quit
//                qDebug() << peerToNameMap[*localhost] << "Sent message after receiving status" << size;
                sendMessage(gossipAboutPeer, messages[gossipAboutPeer][size - 1], size, currentPeer);
                return 0;
            }
        }

        for (it = wantMap.begin(); it != wantMap.end(); ++it) {
            QString gossipAboutName = it.key();
            int size = it.value().toInt();

            Peer gossipAboutPeer = nameToPeerMap[gossipAboutName];

            if (messages[gossipAboutPeer].size() + 1 < size) { // Peer has more info than I do, I'll send dat bitch a status map
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
        //createStatusMap(messages);
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

QVariantMap ChatDialog::createStatusMap(QMap<Peer, QVector<QString> > messages) {
    QVariantMap statusMap;
    QMap<Peer, QVector<QString> >::iterator it;
    for (it = messages.begin(); it != messages.end(); ++it) {
        statusMap.insert(peerToNameMap[it.key()], it.value().size() + 1);
    }

    QVariantMap returnMap;
    returnMap.insert("Want", statusMap);

    printMap(statusMap, peerToNameMap[*localhost]);

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

