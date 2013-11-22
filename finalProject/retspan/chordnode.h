#ifndef CHORDNODE_H
#define CHORDNODE_H

#include <QObject>

#include "networkmanager.h"
#include "node.h"

class ChordNode : public QObject
{
    Q_OBJECT
public:
    explicit ChordNode(NetworkManager *chordManager, Node *localhost, QObject *parent = 0);

signals:

public slots:
    void receiveData();

private:
    NetworkManager *chordManager;
    Node *localhost;
};

#endif // CHORDNODE_H
