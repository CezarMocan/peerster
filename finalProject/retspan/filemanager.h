#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QMap>

#include "networkmanager.h"
#include "keyvaluestore.h"
#include "mainwindow.h"
#include "chordnode.h"

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(NetworkManager *networkManager, KeyValueStore *kvs, MainWindow *mainWindow, ChordNode *chordNode, QObject *parent = 0);
    void downloadFile(QString fileID, Node owner, QString fileName, bool goToKVS);
    void downloadFileReplica(QString fileID, Node owner, QString fileName, bool goToKVS);
    void uploadFile(QString fileID, Node owner);
    void uploadFileReplica(QString fileID, Node owner);

signals:


public slots:   
    void receivedDownloadRequest(Node from, QString fileID);
    void receivedDownloadReply(QString fileID, quint32 fileSize, quint32 noBlocks);
    void receivedDownloadBlockRequest(Node from, QString fileID, quint32 block);
    void receivedDownloadBlockReply(QString fileID, quint32 block, QByteArray blockContents);
    void receivedUploadNotification(Node from, QString fileID);       
    void receivedKeysMotherfucker(Node from);

    void receivedDownloadRequestReplica(Node from, QString fileID);
    void receivedDownloadReplyReplica(QString fileID, quint32 fileSize, quint32 noBlocks);
    void receivedDownloadBlockRequestReplica(Node from, QString fileID, quint32 block);
    void receivedDownloadBlockReplyReplica(QString fileID, quint32 block, QByteArray blockContents);
    void receivedUploadNotificationReplica(Node from, QString fileID);

    void replicateShitToPredecessor(Node predecessor);
    void updateKeysInSuccessor(Node successor);

private:
    NetworkManager *networkManager;
    KeyValueStore *kvs;
    MainWindow *mainWindow;
    ChordNode *chordNode;

    QMap<QString, bool> goesToKVS;
    QMap<QString, bool> removeFromKVS;
    QMap<QString, QString> sentDownloadRequest;
    QMap<QString, quint32> fileSizeMap;
    QMap<QString, quint32> noBlocksMap;
    QMap<QString, quint32> currentBlockMap;
    QMap<QString, QByteArray> fileContentsMap;
    QMap<QString, QString> fileNameMap;
    QMap<QString, Node> ownerMap;

    QMap<QString, bool> goesToKVSReplica;
    QMap<QString, bool> removeFromKVSReplica;
    QMap<QString, QString> sentDownloadRequestReplica;
    QMap<QString, quint32> fileSizeMapReplica;
    QMap<QString, quint32> noBlocksMapReplica;
    QMap<QString, quint32> currentBlockMapReplica;
    QMap<QString, QByteArray> fileContentsMapReplica;
    QMap<QString, QString> fileNameMapReplica;
    QMap<QString, Node> ownerMapReplica;

};

#endif // FILEMANAGER_H
