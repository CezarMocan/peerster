#include <QtCrypto>

#include "chordnode.h"

ChordNode::ChordNode(NetworkManager *chordManager, Node *localhost, QObject *parent) : QObject(parent) {
    this->chordManager = chordManager;
    connect(this->chordManager, SIGNAL(receivedChordQuery(Node,QString)), this, SLOT(receivedChordQuery(Node,QString)));
    connect(this->chordManager, SIGNAL(receivedChordReply(QString,Node)), this, SLOT(receivedChordReply(QString,Node)));

    this->localhost = localhost;

    this->fingerTable = new QVector<Node>(Util::KEYSPACE_SIZE);
    this->neighbourTable = new QVector<Node>(Util::KEYSPACE_SIZE);
}


Node ChordNode::findFingerSuccessor(QString key) { //TODO: this shit could use some improvement
    QString bestResult = Util::getCircleDifference(localhost->getID(), key);
    Node bestNode = (*localhost);

    for (int i = 0; i < fingerTable->size(); i++) {
        QString fingerID = fingerTable->at(i).getID();
        if (fingerID == "")
            continue;
        QString currentResult = Util::getCircleDifference(fingerID, key);
        if (currentResult < bestResult) {
            bestResult = currentResult;
            bestNode = fingerTable->at(i);
        }
    }

    return bestNode;
}

void ChordNode::receivedChordQuery(Node from, QString key) {
    Node fingerSuccessor = findFingerSuccessor(key);
    if (fingerSuccessor == (*localhost)) {
        QByteArray datagram = Util::serializeVariantMap(Util::createChordReply(key, *localhost));
        chordManager->sendData(from, datagram);
    } else {
        QByteArray datagram = Util::serializeVariantMap(Util::createChordQuery(from, key));
        chordManager->sendData(fingerSuccessor, datagram);
    }
}

void ChordNode::receivedChordReply(QString key, Node value) {
    emit receivedReplyFromChord(key, value);
}

