#include <unistd.h>

#include <QDebug>

#include "netsocket.hh"

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
