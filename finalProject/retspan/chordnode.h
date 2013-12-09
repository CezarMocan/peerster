#ifndef CHORDNODE_H
#define CHORDNODE_H

#include <QObject>
#include <QVector>
#include <QHash>

#include "networkmanager.h"
#include "node.h"
#include "util.h"
#include "fingerentry.h"

class ChordNode : public QObject
{
    Q_OBJECT
public:
    enum State {INITIALIZING, UPDATING_OTHERS, READY};

    explicit ChordNode(NetworkManager *chordManager, Node *localhost, QObject *parent = 0);    
    Node findFingerSuccessor(QString key);
    Node findFingerPredecessor(QString key);

signals:
    void receivedReplyFromChord(QString key, Node value);
    void stateUpdateUpdatingOthers();
    void stateUpdateReady();
    void updatedFingerTable(QVector<FingerEntry> fingerTable);
    void updatedPredecessor(Node predecessor);

public slots:
    // UI slots
    void connectButtonPushed(Node neighbour);
    // Chord backend slots
    void receivedChordQuery(Node from, QString key);
    void receivedChordReply(QString key, Node value);
    void receivedGetPredecessorRequest(Node from, int position);
    void receivedGetPredecessorReply(Node neighbour, Node predecessor, int position);
    void receivedUpdatePredecessor(Node newPredecessor);
    void receivedUpdateFinger(Node newFinger, int position);
    void receivedChordQueryPred(Node from, QString key);
    void receivedChordReplyPred(QString key, Node value);


private:
    void createFingerTable();
    void initFingerTable(Node neighbour, int position);
    void createNeighbourTable();
    void updateOthers(int position);
    void printFingerTable();

    NetworkManager *chordManager;
    Node *localhost;
    QVector<FingerEntry> *fingerTable;
    QVector<Node> *neighbourTable;    
    State state;
    Node predecessor;
    Node neighbour;

    QHash<QString, int> sentQueries;
    QHash<QString, int> sentUpdateOthers;
};

#endif // CHORDNODE_H
