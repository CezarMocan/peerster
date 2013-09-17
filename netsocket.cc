#include <unistd.h>

#include <QDebug>

#include "netsocket.hh"

const QString NetSocket::DEFAULT_TEXT_KEY = QString("ChatText");
const QString NetSocket::DEFAULT_ORIGIN_KEY = QString("Origin");
const QString NetSocket::DEFAULT_SEQ_NO_KEY = QString("SeqNo");
const QString NetSocket::DEFAULT_WANT_KEY = QString("Want");

NetSocket::NetSocket() {
	// Pick a range of four UDP ports to try to allocate by default,
	// computed based on my Unix user ID.
	// This makes it trivial for up to four Peerster instances per user
	// to find each other on the same host,
	// barring UDP port conflicts with other applications
	// (which are quite possible).
	// We use the range from 32768 to 49151 for this purpose.
	myPortMin = 32768 + (getuid() % 4096)*4;
	myPortMax = myPortMin + 3;
    currentPort = -1;
}

bool NetSocket::bind() {
	// Try to bind to each of the range myPortMin..myPortMax in turn.
	for (int p = myPortMin; p <= myPortMax; p++) {
		if (QUdpSocket::bind(p)) {
			qDebug() << "bound to UDP port " << p;
            currentPort = p;
			return true;
		}
	}

	qDebug() << "Oops, no ports in my default range " << myPortMin
		<< "-" << myPortMax << " available";
	return false;
}

void NetSocket::writeDatagramPeerList(QByteArray *serializedMessage, QVector<Peer> peerList) {
    for (int p = 0; p < peerList.size(); p++) {
        writeDatagramSinglePeer(serializedMessage, peerList[p]);
    }
}

void NetSocket::writeDatagramSinglePeer(QByteArray *serializedMessage, Peer to) {
    if (QUdpSocket::writeDatagram(*serializedMessage, to.hostAddress, to.port) != -1) {
 //       qDebug() << "Successfully sent to host: " << to.hostAddress << ":" << to.port;
    } else {
        qDebug() << "Failed to send to host: " << to.hostAddress << ":" << to.port;
    }
}

// Returns a QVector with the port numbers between myPortMin and
// myPortMax, different than the port of the current socket, on localhost
QVector<int> NetSocket::getLocalhostPorts() {
    if (currentPort == -1) {
        qDebug() << "Warining: socket not bound to any port";
    }

    QVector<int> ports;

    for (int p = myPortMin; p <= myPortMax; p++) {
        if (p != currentPort)
            ports.push_back(p);
    }

    return ports;
}

int NetSocket::getCurrentPort() {
    return currentPort;
}

QVariantMap NetSocket::serializeMessage(QString fromName, QString text, int position) {
    // Create the QVariantMap containing the message
    QVariantMap textVariantMap;
    textVariantMap.clear();
    textVariantMap.insert(DEFAULT_TEXT_KEY, QVariant(text));
    textVariantMap.insert(DEFAULT_ORIGIN_KEY, QVariant(fromName));
    textVariantMap.insert(DEFAULT_SEQ_NO_KEY, QVariant(position));

    return textVariantMap;
}

QByteArray NetSocket::serializeVariantMap(QVariantMap map) {
    QByteArray *serializedMessage = new QByteArray();

    QDataStream *serializer = new QDataStream(serializedMessage, QIODevice::WriteOnly);
    (*serializer) << map;

    return (*serializedMessage);
}

// Sends message to all peer list
void NetSocket::sendMessage(QString from, QString message, int position, QVector<Peer> peerList) {
    QByteArray serializedMessage = serializeVariantMap(serializeMessage(from, message, position));
    writeDatagramPeerList(&serializedMessage, peerList);
}

// Sends message only to specified peer
void NetSocket::sendMessage(QString from, QString message, int position, Peer to) {
    QByteArray serializedMessage = serializeVariantMap(serializeMessage(from, message, position));
    writeDatagramSinglePeer(&serializedMessage, to);
}

// Sends status message to specified peer
void NetSocket::sendMessage(QVariantMap status, Peer to) {
    QByteArray serializedMessage = serializeVariantMap(status);
    writeDatagramSinglePeer(&serializedMessage, to);
}

void NetSocket::sendStatus(Peer from, QMap<QString, QVector<QString> > messages) {
    QVariantMap status = createStatusMap(messages);
    sendMessage(status, from);
}

QVariantMap NetSocket::createStatusMap(QMap<QString, QVector<QString> > messages) {
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


