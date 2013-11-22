#include <QtCrypto>

#include "chordnode.h"

ChordNode::ChordNode(NetworkManager *chordManager, Node *localhost, QObject *parent) : QObject(parent) {
    this->chordManager = chordManager;
    this->localhost = localhost;
    connect(chordManager, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

void ChordNode::receiveData() {
    while (chordManager->hasPendingDatagrams()) {
        QByteArray *datagram = new QByteArray();
        datagram->resize(chordManager->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;
        chordManager->readDatagram(datagram->data(), datagram->size(), &sender, &senderPort);

        // Process datagram
    }
}
