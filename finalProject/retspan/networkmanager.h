#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QUdpSocket>
#include <QObject>

#include "node.h"

class NetworkManager : public QUdpSocket
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = 0);
    bool bind();
    void sendData(Node remotePeer, QByteArray data);
    quint16 getLocalPort();
    Node* getLocalhost();

signals:
    void receivedChordQuery(Node from, QString key);
    void receivedChordReply(QString key, Node value);
    void receivedGetPredecessorRequest(Node from, int position);
    void receivedGetPredecessorReply(Node neighbour, Node predecessor, int position);
    void receivedUpdatePredecessor(Node newPredecessor);
    void receivedUpdateFinger(Node newFinger, int position);
    void receivedChordQueryPred(Node from, QString key);
    void receivedChordReplyPred(QString key, Node value);
    void receivedKeywordQuery(Node from, QString keyword);
    void receivedKeywordReply(QString keyword, QVariantList ids, QVariantList names);
    void receivedKeywordUpdate(QString keyword, QString fileID, QString fileName);


public slots:
    void receiveData();

private:
    quint16 myPortMin;
    quint16 myPortMax;
    quint16 currentPort;
    Node *localhost;
};

#endif // NETWORKMANAGER_H
