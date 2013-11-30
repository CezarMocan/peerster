#include <QDebug>

#include "networkmanager.h"
#include "node.h"
#include "util.h"

NetworkManager::NetworkManager(QObject *parent) : QUdpSocket(parent) {
    myPortMin = 32768 + (getuid() % 4096)*4;
    myPortMax = myPortMin + 10;
    currentPort = -1;
    localhost = NULL;
    connect(this, SIGNAL(readyRead()), this, SLOT(receiveData()));
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

void NetworkManager::receiveData() {
    while (this->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        datagram->resize(this->pendingDatagramSize());

        QHostAddress senderAddress;
        quint16 senderPort;
        this->readDatagram(datagram->data(), datagram->size(), &senderAddress, &senderPort);

        Node sender(senderAddress, senderPort);

        QVariantMap variantMap;

        QDataStream *deserializer = new QDataStream(datagram, QIODevice::ReadOnly);
        (*deserializer) >> variantMap;

        Node node; QString key; QString type;
        Util::parseChordVariantMap(variantMap, type, node);

        if (type == Util::CHORD_QUERY) {
            key = variantMap[Util::KEY].toString();
            emit receivedChordQuery(node, key);
        }
        else if (type == Util::CHORD_REPLY) {
            key = variantMap[Util::KEY].toString();
            emit receivedChordReply(key, node);
        }
        else if (type == Util::GET_PREDECESSOR) {
            int position = variantMap[Util::POSITION].toInt();
            emit receivedGetPredecessorRequest(node, position);
        } else if (type == Util::GET_PREDECESSOR_REPLY) {
            int position = variantMap[Util::POSITION].toInt();
            emit receivedGetPredecessorReply(sender, node, position);
        } else if (type == Util::UPDATE_PREDECESSOR) {
            emit receivedUpdatePredecessor(node);
        } else if (type == Util::UPDATE_FINGER) {
            int position = variantMap[Util::POSITION].toInt();
            emit receivedUpdateFinger(node, position);
        }
        else {
            qDebug() << "Received message of unsupported type " + type;
        }
    }
}

Node* NetworkManager::getLocalhost() {
    return this->localhost;
}

