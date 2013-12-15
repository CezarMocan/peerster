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

    void receivedDownloadRequest(Node from, QString fileID);
    void receivedDownloadReply(QString fileID, quint32 fileSize, quint32 noBlocks);
    void receivedDownloadBlockRequest(Node from, QString fileID, quint32 block);
    void receivedDownloadBlockReply(QString fileID, quint32 block, QByteArray blockContents);

    void receivedUploadNotification(Node from, QString fileID);

    void receivedKeysMotherfucker(Node from);

    void receivedUAliveBoss(Node from);
    void receivedYesBoss(Node from);

    void receivedKeywordUpdateReplica(QString keyword, QString fileID, QString fileName);
    void receivedDownloadRequestReplica(Node from, QString fileID);
    void receivedDownloadReplyReplica(QString fileID, quint32 fileSize, quint32 noBlocks);
    void receivedDownloadBlockRequestReplica(Node from, QString fileID, quint32 block);
    void receivedDownloadBlockReplyReplica(QString fileID, quint32 block, QByteArray blockContents);
    void receivedUploadNotificationReplica(Node from, QString fileID);

public slots:
    void receiveData();

private:
    quint16 myPortMin;
    quint16 myPortMax;
    quint16 currentPort;
    Node *localhost;
};

#endif // NETWORKMANAGER_H
