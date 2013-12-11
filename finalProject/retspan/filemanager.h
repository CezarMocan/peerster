#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QMap>

#include "networkmanager.h"
#include "keyvaluestore.h"

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(NetworkManager *networkManager, KeyValueStore *kvs, QObject *parent = 0);
    void downloadFile(QString fileID, Node owner, QString fileName, bool goToKVS);
    void uploadFile(QString fileID, Node owner);

signals:


public slots:   
    void receivedDownloadRequest(Node from, QString fileID);
    void receivedDownloadReply(QString fileID, quint32 fileSize, quint32 noBlocks);
    void receivedDownloadBlockRequest(Node from, QString fileID, quint32 block);
    void receivedDownloadBlockReply(QString fileID, quint32 block, QByteArray blockContents);
    void receivedUploadNotification(Node from, QString fileID);

private:
    NetworkManager *networkManager;
    KeyValueStore *kvs;

    QMap<QString, bool> goesToKVS;
    QMap<QString, bool> removeFromKVS;

    QMap<QString, QString> sentDownloadRequest;

    QMap<QString, quint32> fileSizeMap;
    QMap<QString, quint32> noBlocksMap;
    QMap<QString, quint32> currentBlockMap;
    QMap<QString, QByteArray> fileContentsMap;
    QMap<QString, QString> fileNameMap;

    QMap<QString, Node> ownerMap;

};

#endif // FILEMANAGER_H
