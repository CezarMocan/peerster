#include <QtCrypto>

#include "chordnode.h"

ChordNode::ChordNode(NetworkManager *chordManager, Node *localhost, QObject *parent) : QObject(parent) {
    this->chordManager = chordManager;
    connect(this->chordManager, SIGNAL(receivedChordQuery(Node,QString)), this, SLOT(receivedChordQuery(Node,QString)));
    connect(this->chordManager, SIGNAL(receivedChordReply(QString,Node)), this, SLOT(receivedChordReply(QString,Node)));
    connect(this->chordManager, SIGNAL(receivedGetPredecessorReply(Node,Node,int)), this, SLOT(receivedGetPredecessorReply(Node,Node,int)));
    connect(this->chordManager, SIGNAL(receivedGetPredecessorRequest(Node,int)), this, SLOT(receivedGetPredecessorRequest(Node,int)));
    connect(this->chordManager, SIGNAL(receivedUpdatePredecessor(Node)), this, SLOT(receivedUpdatePredecessor(Node)));
    connect(this->chordManager, SIGNAL(receivedUpdateFinger(Node,int)), this, SLOT(receivedUpdateFinger(Node,int)));

    this->localhost = localhost;    
    this->state = INITIALIZING;
    this->predecessor = *localhost;

    createFingerTable();
    createNeighbourTable();
}

void ChordNode::createFingerTable() {
    this->fingerTable = new QVector<FingerEntry>(Util::KEYSPACE_SIZE);
    QString powerOfTwo = Util::ONE;
    QString nextPowerOfTwoMinusOne = powerOfTwo;

    for (int i = 0; i < Util::KEYSPACE_SIZE; i++) {
        (*fingerTable)[i].start = Util::addition(localhost->getID(), powerOfTwo);
        (*fingerTable)[i].stop = Util::addition(localhost->getID(), nextPowerOfTwoMinusOne);
        (*fingerTable)[i].succ = (*localhost);

        //qDebug() << ((*fingerTable)[i].start) << " " << ((*fingerTable)[i].stop) << "\n";

        QString aux = Util::addition(powerOfTwo, powerOfTwo);
        powerOfTwo = aux;
        aux = Util::addition(nextPowerOfTwoMinusOne, nextPowerOfTwoMinusOne);
        nextPowerOfTwoMinusOne = Util::addition(aux, Util::ONE);
    }
}

void ChordNode::connectButtonPushed(Node neighbour) {
    this->neighbour = neighbour;
    initFingerTable(neighbour, 0);
}

void ChordNode::createNeighbourTable() {
    this->neighbourTable = new QVector<Node>(Util::KEYSPACE_SIZE);
}

Node ChordNode::findFingerSuccessor(QString key) { //TODO: this shit could use some improvement
    QString bestResult = Util::getCircleDifference(localhost->getID(), key);
    Node bestNode = (*localhost);

    for (int i = 0; i < fingerTable->size(); i++) {
        QString fingerID = fingerTable->at(i).succ.getID();
        if (fingerID == "")
            continue;
        QString currentResult = Util::getCircleDifference(fingerID, key);
        if (currentResult < bestResult) {
            bestResult = currentResult;
            bestNode = fingerTable->at(i).succ;
        }
    }

    return bestNode;
}

Node ChordNode::findFingerPredecessor(QString key) { //TODO: this shit could use some improvement
    QString bestResult = Util::getCircleDifference(key, localhost->getID());
    Node bestNode = (*localhost);

    for (int i = 0; i < fingerTable->size(); i++) {
        QString fingerID = fingerTable->at(i).succ.getID();
        if (fingerID == "")
            continue;
        QString currentResult = Util::getCircleDifference(key, fingerID);
        if (currentResult < bestResult) {
            bestResult = currentResult;
            bestNode = fingerTable->at(i).succ;
        }
    }

    return bestNode;
}

void ChordNode::initFingerTable(Node neighbour, int position) {
    qDebug() << "Running init finger table for position " << position << " neighbour is " << neighbour.toString();
    QByteArray datagram = Util::serializeVariantMap(Util::createChordQuery(*localhost, (*fingerTable)[position].start));
    sentQueries.insert((*fingerTable)[position].start, position);
    chordManager->sendData(neighbour, datagram);
}

void ChordNode::updateOthers() {

    QString powerOfTwo = Util::ONE;

    for (int i = 0; i < Util::KEYSPACE_SIZE; i++) {
        QString currentKey = Util::getCircleDifference(localhost->getID(), powerOfTwo);
        QByteArray datagram = Util::serializeVariantMap(Util::createChordQuery(*localhost, currentKey));

        sentUpdateOthers.insert(currentKey, i);
        chordManager->sendData(*localhost, datagram);
        QString aux = Util::addition(powerOfTwo, powerOfTwo);
        powerOfTwo = aux;
    }
}

void ChordNode::receivedChordQuery(Node from, QString key) {
    Node fingerSuccessor = findFingerSuccessor(key);
    if (fingerSuccessor == (*localhost)) {
        QByteArray datagram = Util::serializeVariantMap(Util::createChordReply(key, *localhost));
        chordManager->sendData(from, datagram);
    } else {
        Node fingerPredecessor = findFingerPredecessor(key);
        QByteArray datagram = Util::serializeVariantMap(Util::createChordQuery(from, key));
        chordManager->sendData(fingerPredecessor, datagram);
    }
}

void ChordNode::receivedChordReply(QString key, Node value) {
    if (this->state == INITIALIZING) {
        if (sentQueries.find(key) == sentQueries.end()) {
            qDebug() << "Received reply for a key that I did not request! " << key << "\n";
            return;
        }

        // Ask for predecessor and then update finger table
        int position = sentQueries.find(key).value();
        sentQueries.remove(key);

        QByteArray datagram = Util::serializeVariantMap(Util::createGetPredecessor(*localhost, position));
        chordManager->sendData(value, datagram);

        (*fingerTable)[position].succ = value;

        // send next query here; change state to READY after finishing with all the queries
        for (int i = position + 1; i < Util::KEYSPACE_SIZE; i++) {
            if (Util::intervalContainsKey(localhost->getID(), (*fingerTable)[i - 1].succ.getID(), (*fingerTable)[i].start)) {
                (*fingerTable)[i].succ = (*fingerTable)[i - 1].succ;
            } else {
                initFingerTable(this->neighbour, i);
                return;
            }
        }

        this->state = UPDATING_OTHERS;
        qDebug() << "State UPDATING_OTHERS for node " << this->localhost->toString();

        updateOthers();
    } else if (this->state == UPDATING_OTHERS) {
        if (sentUpdateOthers.find(key) == sentUpdateOthers.end()) {
            qDebug() << "Received reply for a key that I did not request in update others state! " << key << "\n";
            return;
        }

        int position = sentUpdateOthers.find(key).value();
        sentUpdateOthers.remove(key);

        // Find the predecessor of the node that's responsible for this key and then update its finger table with the current node
        QByteArray datagram = Util::serializeVariantMap(Util::createGetPredecessor(*localhost, position));
        chordManager->sendData(value, datagram);
    } else {
        emit receivedReplyFromChord(key, value);
    }
}

void ChordNode::receivedGetPredecessorRequest(Node from, int position) {
    if (this->state == INITIALIZING || this->state == UPDATING_OTHERS) {
        QByteArray datagram = Util::serializeVariantMap(Util::createGetPredecessorReply(this->predecessor, position));
        chordManager->sendData(from, datagram);
    } else {

    }
}

void ChordNode::receivedGetPredecessorReply(Node neighbour, Node predecessor, int position) {
    qDebug() << "Received get predecessor reply from " << neighbour.toString() << " predecessor is " << predecessor.toString();
    if (this->state == INITIALIZING) {
        if (position == 0) {
            this->predecessor = predecessor;
            QByteArray datagram = Util::serializeVariantMap(Util::createUpdatePredecessor(*localhost));
            chordManager->sendData(neighbour, datagram);
        } else {
            qDebug() << "Received predecessor reply for position != 0! " << position << "\n";
            return;
        }
    } else if (this->state == UPDATING_OTHERS) {
        QByteArray datagram = Util::serializeVariantMap(Util::createUpdateFinger(*localhost, position));
        chordManager->sendData(predecessor, datagram);
    } else {

    }
}

void ChordNode::receivedUpdatePredecessor(Node newPredecessor) {
    this->predecessor = newPredecessor;
    qDebug() << "Updated predecessor to " << newPredecessor.toString();
}

void ChordNode::receivedUpdateFinger(Node newFinger, int position) {
    if (Util::intervalContainsKey(this->localhost->getID(), (*fingerTable)[position].succ.getID(), newFinger.getID())) {
        (*fingerTable)[position].succ = newFinger;
        QByteArray datagram = Util::serializeVariantMap(Util::createUpdateFinger(newFinger, position));
        chordManager->sendData(this->predecessor, datagram);
    }
}

