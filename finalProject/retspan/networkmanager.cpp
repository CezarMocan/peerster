#include <QDebug>

#include "networkmanager.h"
#include "node.h"

NetworkManager::NetworkManager(QObject *parent) : QUdpSocket(parent) {
    myPortMin = 32768 + (getuid() % 4096)*4;
    myPortMax = myPortMin + 10;
    currentPort = -1;
    localhost = NULL;
}

bool NetworkManager::bind() {
    // Try to bind to each of the range myPortMin..myPortMax in turn.
    for (int p = myPortMin; p <= myPortMax; p++) {
        if (QUdpSocket::bind(p)) {
            qDebug() << "bound to UDP port " << p;
            currentPort = p;
            localhost = new Node(QHostAddress::LocalHost, currentPort);
            return true;
        }
    }

    qDebug() << "Oops, no ports in my default range " << myPortMin
        << "-" << myPortMax << " available";
    return false;
}

quint16 NetworkManager::getLocalPort() {
    return this->currentPort;
}

void NetworkManager::sendData(Node remotePeer, QByteArray data) {
    if (QUdpSocket::writeDatagram(data, remotePeer.getAddress(), remotePeer.getPort()) == -1)
        qDebug() << "Failed to send data to " << remotePeer.toString();
}

Node* NetworkManager::getLocalhost() {
    return this->localhost;
}

