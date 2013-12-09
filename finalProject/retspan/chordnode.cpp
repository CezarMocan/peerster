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
    connect(this->chordManager, SIGNAL(receivedChordQueryPred(Node,QString)), this, SLOT(receivedChordQueryPred(Node,QString)));
    connect(this->chordManager, SIGNAL(receivedChordReplyPred(QString,Node)), this, SLOT(receivedChordReplyPred(QString,Node)));


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

Node ChordNode::findFingerPredecessor(QString key) { //TODO: this shit could use some improvement
    QString bestResult = Util::getCircleDifference(localhost->getID(), key);
    Node bestNode = (*localhost);

    /*
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
    */

    if (!(this->predecessor == *localhost) && Util::intervalContainsKey(this->predecessor.getID(), localhost->getID(), key))
        return this->predecessor;
    for (int i = 0; i < fingerTable->size(); i++) {
        if (Util::intervalContainsKey(localhost->getID(), key, fingerTable->at(i).succ.getID()))
            return fingerTable->at(i).succ;
    }

    return (*localhost);
}

Node ChordNode::findFingerSuccessor(QString key) { //TODO: this shit could use some improvement
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
    qDebug() << "Running init finger table for position " << position << " neighbour is " << neighbour.toString() << " key is " << (*fingerTable)[position].start;
    QByteArray datagram = Util::serializeVariantMap(Util::createChordQuery(*localhost, (*fingerTable)[position].start));
    sentQueries.insert((*fingerTable)[position].start, position);
    chordManager->sendData(neighbour, datagram);
}

void ChordNode::updateOthers(int position) {
    if (position >= Util::KEYSPACE_SIZE) {
        for (int i = 0; i < Util::KEYSPACE_SIZE; i++) {
            if (Util::intervalContainsKey(this->predecessor.getID(), localhost->getID(), fingerTable->at(i).start))
                (*fingerTable)[i].succ = *localhost;
        }
        printFingerTable();
        qDebug() << "State is READY!";
        this->state = READY;
        emit(stateUpdateReady());
        //TODO: Key exchange
        return;
    }

    qDebug() << "Update others position = " << position;

    QString powerOfTwo = Util::ONE;

    for (int i = 1; i <= position; i++) {
        QString aux = Util::addition(powerOfTwo, powerOfTwo);
        powerOfTwo = aux;
    }

    QString currentKey = Util::difference(localhost->getID(), powerOfTwo);
    qDebug() << "Current key = " << currentKey << " power of 2 = " << powerOfTwo;
    qDebug() << "Creating chord_pred_query in updateOthers";
    QByteArray datagram = Util::serializeVariantMap(Util::createChordQueryPred(*localhost, currentKey));

    sentUpdateOthers.insert(currentKey, position);
    chordManager->sendData(neighbour, datagram);
}

void ChordNode::receivedChordQueryPred(Node from, QString key) {
    qDebug() << "Received chord_query_pred from " << from.toString() << "key = " << key;
    Node fingerPredecessor = findFingerPredecessor(key);
    if (fingerPredecessor == (*localhost) || key == localhost->getID()) {
        QByteArray datagram = Util::serializeVariantMap(Util::createChordReplyPred(key, *localhost));
        qDebug() << "    response is " << localhost->toString();
        chordManager->sendData(from, datagram);
    } else {
        qDebug() << "Creating chord query in receivedChordQueryPred to " << fingerPredecessor.toString();
        QByteArray datagram = Util::serializeVariantMap(Util::createChordQueryPred(from, key));
        chordManager->sendData(fingerPredecessor, datagram);
    }
}

void ChordNode::receivedChordReplyPred(QString key, Node value) {
    qDebug() << "Received chord reply pred for key " << key << " value = " << value.toString();
    if (this->state == UPDATING_OTHERS) {
        if (sentUpdateOthers.find(key) == sentUpdateOthers.end()) {
            qDebug() << "Received reply for a key that I did not request in update others state! " << key << "\n";
            return;
        }

        int position = sentUpdateOthers.find(key).value();
        sentUpdateOthers.remove(key);

        qDebug() << "Received chord predecessor reply for key = " << key << " " << value.toString();
        if (value == *localhost) {
            updateOthers(Util::KEYSPACE_SIZE);
            return;
        }
        QByteArray datagram = Util::serializeVariantMap(Util::createUpdateFinger(*localhost, position));
        chordManager->sendData(value, datagram);
        updateOthers(position + 1);
    }
}

void ChordNode::receivedChordQuery(Node from, QString key) {
    qDebug() << "received chord query from " << from.toString() << " key = " << key;

    Node fingerPredecessor = findFingerPredecessor(key);
    Node successor = fingerTable->at(0).succ;

    if (fingerPredecessor == (*localhost)) {
        QByteArray datagram = Util::serializeVariantMap(Util::createChordReply(key, successor));
        qDebug() << "    response is " << successor.toString();
        chordManager->sendData(from, datagram);
    } else {
        qDebug() << "Creating chord query in receivedChordQuery to " << fingerPredecessor.toString();
        QByteArray datagram = Util::serializeVariantMap(Util::createChordQuery(from, key));
        chordManager->sendData(fingerPredecessor, datagram);
    }
}

void ChordNode::receivedChordReply(QString key, Node value) {
    qDebug() << "Received chord reply for key " << key << " value = " << value.toString();
    if (this->state == INITIALIZING) {
        if (sentQueries.find(key) == sentQueries.end()) {
            qDebug() << "Received reply for a key that I did not request! " << key << "\n";
            return;
        }

        // Ask for predecessor and then update finger table
        int position = sentQueries.find(key).value();
        sentQueries.remove(key);
        qDebug() << "    Position is " << position;

        if (position == 0) {
            qDebug() << "    Creating get predecessor request to " << value.toString();
            QByteArray datagram = Util::serializeVariantMap(Util::createGetPredecessor(*localhost, position));
            chordManager->sendData(value, datagram);
        }

        // Update finger table
        //QString start = fingerTable->at(position).start;
        //if (Util::getCircleDifference(start, value.getID()) < Util::getCircleDifference(start, localhost->getID()))
            (*fingerTable)[position].succ = value;
        //else
//            (*fingerTable)[position].succ = *localhost;

        // send next query here; change state to UPDATE_OTHERS after finishing with all the queries
        for (int i = position + 1; i < Util::KEYSPACE_SIZE; i++) {
            qDebug() << "receivedChordReply for position " << i;
            if (Util::intervalContainsKey(localhost->getID(), (*fingerTable)[i - 1].succ.getID(), (*fingerTable)[i].start)) {
                (*fingerTable)[i].succ = (*fingerTable)[i - 1].succ;
            } else {
                initFingerTable(this->neighbour, i);
                return;
            }
        }
        printFingerTable();

        // Start sending updates to the other nodes only after predecessor has been updated
        if (!(this->predecessor == *localhost)) {
            this->state = UPDATING_OTHERS;
            emit(stateUpdateUpdatingOthers());
            qDebug() << "State UPDATING_OTHERS for node " << this->localhost->toString();
            updateOthers(0);
        }
    } else if (this->state == UPDATING_OTHERS) {
        // wind.. vjhhhhhhhh
    } else {
        //emit receivedReplyFromChord(key, value);
    }
}

void ChordNode::receivedGetPredecessorRequest(Node from, int position) {
    qDebug() << "Got get predecessor request from " << from.toString() << " predecessor is " << this->predecessor.toString() << "\n";

    QByteArray datagram = Util::serializeVariantMap(Util::createGetPredecessorReply(this->predecessor, position));
    chordManager->sendData(from, datagram);
}

void ChordNode::receivedGetPredecessorReply(Node neighbour, Node predecessor, int position) {
    qDebug() << "Received get predecessor reply from " << neighbour.toString() << " predecessor is " << predecessor.toString();
    qDebug() << "Position is " << position;
    if (this->state == INITIALIZING) {
        if (position == 0) {
            qDebug() << "Updating predecessors! ";
            this->predecessor = Node(predecessor);
            QByteArray datagram = Util::serializeVariantMap(Util::createUpdatePredecessor(*localhost));
            chordManager->sendData(neighbour, datagram);

            if (this->state == INITIALIZING && sentQueries.isEmpty()) {
                this->state = UPDATING_OTHERS;
                emit(stateUpdateUpdatingOthers());
                qDebug() << "State UPDATING_OTHERS for node " << this->localhost->toString();
                updateOthers(0);
            }
        } else {
            qDebug() << "Received predecessor reply for position != 0! " << position << "\n";
            return;
        }
    } else if (this->state == UPDATING_OTHERS) {
        // wind.. vjhhh
    } else {

    }
}

void ChordNode::receivedUpdatePredecessor(Node newPredecessor) {
    this->predecessor = newPredecessor;
    qDebug() << "Updated predecessor to " << newPredecessor.toString();
    printFingerTable();
}

void ChordNode::receivedUpdateFinger(Node newFinger, int position) {
    qDebug() << "Received update finger for position " << position << "with node " << newFinger.toString();
    qDebug() << "    " << this->localhost->getID() << (*fingerTable)[position].succ.getID() << newFinger.getID();

    if (Util::intervalContainsKey(this->localhost->getID(), (*fingerTable)[position].succ.getID(), newFinger.getID())) {
        (*fingerTable)[position].succ = newFinger;
        QByteArray datagram = Util::serializeVariantMap(Util::createUpdateFinger(newFinger, position));
        if (!(newFinger == (this->predecessor)))
            chordManager->sendData(this->predecessor, datagram);
    }

    printFingerTable();
}

void ChordNode::printFingerTable() {
    qDebug() << "Node = " << this->localhost->getID() << "Predecessor = " << this->predecessor.toString();
    for (int i = 0; i < Util::KEYSPACE_SIZE; i++) {
        qDebug() << i << ":" << (*fingerTable)[i].start << "-" << (*fingerTable)[i].stop << "->" << (*fingerTable)[i].succ.toString();
    }
}

