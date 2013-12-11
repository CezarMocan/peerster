#include "filemanager.h"
#include "util.h"

FileManager::FileManager(NetworkManager *networkManager, KeyValueStore *kvs, QObject *parent) : QObject(parent) {
    this->networkManager = networkManager;
    connect(this->networkManager, SIGNAL(receivedDownloadRequest(Node,QString)), this, SLOT(receivedDownloadRequest(Node,QString)));
    connect(this->networkManager, SIGNAL(receivedDownloadReply(QString,quint32,quint32)), this, SLOT(receivedDownloadReply(QString,quint32,quint32)));
    connect(this->networkManager, SIGNAL(receivedDownloadBlockRequest(Node,QString,quint32)), this, SLOT(receivedDownloadBlockRequest(Node,QString,quint32)));
    connect(this->networkManager, SIGNAL(receivedDownloadBlockReply(QString,quint32,QByteArray)), this, SLOT(receivedDownloadBlockReply(QString,quint32,QByteArray)));
    connect(this->networkManager, SIGNAL(receivedUploadNotification(Node,QString)), this, SLOT(receivedUploadNotification(Node,QString)));

    this->kvs = kvs;
}

void FileManager::downloadFile(QString fileID, Node owner, QString fileName, bool goToKVS) {
    ownerMap[fileID] = owner;
    fileNameMap[fileID] = fileName;
    goesToKVS[fileID] = goToKVS;

    sentDownloadRequest[fileID] = fileName;
    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadRequest(fileID));
    networkManager->sendData(owner, datagram);
    qDebug() << "Sending download request for fileID = " << fileID << " to node " << owner.toString();
}

void FileManager::uploadFile(QString fileID, Node owner) {
    if (owner == *networkManager->getLocalhost())
        return;

    removeFromKVS[fileID] = true;
    QByteArray datagram = Util::serializeVariantMap(Util::createUploadNotification(fileID));
    networkManager->sendData(owner, datagram);
    qDebug() << "Upload file called for " << fileID << " uploading to " << owner.toString();
}

void FileManager::receivedDownloadRequest(Node from, QString fileID) {
    quint32 fileSize = kvs->getFile(fileID).size();
    quint32 noBlocks = kvs->getNoBlocks(fileID);
    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadReply(fileID, fileSize, noBlocks));
    networkManager->sendData(from, datagram);
    qDebug() << "Received download request from " << from.toString() << " for " << fileID;
}

void FileManager::receivedDownloadReply(QString fileID, quint32 fileSize, quint32 noBlocks) {
    qDebug() << "Received download reply for " << fileID << " size = " << fileSize << " noBlocks = " << noBlocks;
}

void FileManager::receivedDownloadBlockRequest(Node from, QString fileID, quint32 block) {

}

void FileManager::receivedDownloadBlockReply(QString fileID, quint32 block, QByteArray blockContents) {

}

void FileManager::receivedUploadNotification(Node from, QString fileID) {
    qDebug() << "Received upload notification from " << from.toString() << " for file " << fileID;
    QString emptyName;
    downloadFile(fileID, from, emptyName, true);
}

