#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QString>

#include "file.h"
#include "netsocket.hh"
#include "peer.hh"

class FileManager : public QObject
{
    Q_OBJECT
public:
    FileManager(NetSocket* sock, QString localhostName);
    void addFile(QString fileName);
    QByteArray getBlockByHash(QByteArray blockHash);
    //File keywordSearch(QStringList keywords);
    QList<QPair<QString, QByteArray> > searchByKeyword(QString keywords);

signals:
    void newBlockToSend(QByteArray fileID, QByteArray block);
    void blockReadyForSending(QByteArray block, QString originName, quint32 blockID);
    void completedTransfer(QByteArray fileID, QString fileName);

public slots:
    void retrieveFile(QByteArray fileID, QString peerName, Peer firstHop, quint32 hopLimit, QString fileName = NULL);
    void gotNewBlockResponse(QString originName, QByteArray repliedBlock, QByteArray data);

private:
    // Socket
    NetSocket* sock;

    // Number of received files
    quint32 noReceivedFiles;

    // Localhost name
    QString localhostName;

    // List of local + received files that are shared by peerster
    QVector<File> sharedFiles;

    // Blocklist received for a fileID request
    QMap<QByteArray, QByteArray> idToBlocklist;

    QMap<QByteArray, QVector<QByteArray> > idToBlocklistArray;

    // Keep a pointer to the next block that has to be requested for a fileID in transfer
    QMap<QByteArray, int> blockNumber;

    // Map by fileID with the number of SHA blocks in file
    QMap<QByteArray, int> fileBlockSize;

    // Keep track of all the blocks received until now for a fileID
    QMap<QByteArray, QVector<QByteArray> > blocksReceived;

    // Remember file name, if request comes from a keyword search
    QMap<QByteArray, QString> idToName;

    QMap<QString, QByteArray> nameToId;

    QMap<QByteArray, QByteArray> hashToBlock;
    QMap<QByteArray, QByteArray> blockToHash;
    QMap<QByteArray, QByteArray> belongsTo;

    void sendRequest(QString originName, int blockNo, QByteArray fileID);
    int parseBlocklist(QByteArray blocklist, QString originName, QByteArray fileID);
    QByteArray getFileID(QByteArray block);
    bool checkSHA(QByteArray hash, QByteArray data);
    void completeTransfer(QByteArray fileID);
};

#endif // FILEMANAGER_H
