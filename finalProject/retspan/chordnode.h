#ifndef CHORDNODE_H
#define CHORDNODE_H

#include <QObject>
#include <QVector>

#include "networkmanager.h"
#include "node.h"
#include "util.h"

class ChordNode : public QObject
{
    Q_OBJECT
public:
    explicit ChordNode(NetworkManager *chordManager, Node *localhost, QObject *parent = 0);
    Node findFingerSuccessor(QString key);

signals:
    void receivedReplyFromChord(QString key, Node value);

public slots:
    void receivedChordQuery(Node from, QString key);
    void receivedChordReply(QString key, Node value);

private:
    NetworkManager *chordManager;
    Node *localhost;
    QVector<Node> *fingerTable;
    QVector<Node> *neighbourTable;
};

#endif // CHORDNODE_H
