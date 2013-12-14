#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "filemanager.h"
#include "util.h"
#include "mainwindow.h"

FileManager::FileManager(NetworkManager *networkManager, KeyValueStore *kvs, MainWindow *mainWindow, QObject *parent) : QObject(parent) {
    this->networkManager = networkManager;
    connect(this->networkManager, SIGNAL(receivedDownloadRequest(Node,QString)), this, SLOT(receivedDownloadRequest(Node,QString)));
    connect(this->networkManager, SIGNAL(receivedDownloadReply(QString,quint32,quint32)), this, SLOT(receivedDownloadReply(QString,quint32,quint32)));
    connect(this->networkManager, SIGNAL(receivedDownloadBlockRequest(Node,QString,quint32)), this, SLOT(receivedDownloadBlockRequest(Node,QString,quint32)));
    connect(this->networkManager, SIGNAL(receivedDownloadBlockReply(QString,quint32,QByteArray)), this, SLOT(receivedDownloadBlockReply(QString,quint32,QByteArray)));
    connect(this->networkManager, SIGNAL(receivedUploadNotification(Node,QString)), this, SLOT(receivedUploadNotification(Node,QString)));
    connect(this->networkManager, SIGNAL(receivedKeysMotherfucker(Node)), this, SLOT(receivedKeysMotherfucker(Node)));

    this->kvs = kvs;
    this->mainWindow = mainWindow;
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
    if (fileSize == 0)
        return;

    qDebug() << "Received download reply for " << fileID << " size = " << fileSize << " noBlocks = " << noBlocks;
    sentDownloadRequest.remove(fileID);

    fileSizeMap[fileID] = fileSize;
    noBlocksMap[fileID] = noBlocks;
    currentBlockMap[fileID] = 0;

    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadBlockRequest(fileID, 0));
    networkManager->sendData(ownerMap[fileID], datagram);
}

void FileManager::receivedDownloadBlockRequest(Node from, QString fileID, quint32 block) {
    qDebug() << "Received download block request for " << fileID << " block #" << block;
    QByteArray blockContents = kvs->getBlock(fileID, block);
    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadBlockReply(fileID, block, blockContents));
    networkManager->sendData(from, datagram);
    if (block == kvs->getNoBlocks(fileID)) {
        if (removeFromKVS[fileID] == true)
            kvs->removeFile(fileID);
        removeFromKVS.remove(fileID);
    }
}

void FileManager::receivedDownloadBlockReply(QString fileID, quint32 block, QByteArray blockContents) {
    qDebug() << "Received download block reply for " << fileID << " with block #" << block;
    if (block == noBlocksMap[fileID]) {
        qDebug() << "Download complete!";
        if (goesToKVS[fileID]) {
            qDebug() << "Added to KVS";
            kvs->addFile(fileID, fileContentsMap[fileID]);
        } else {
            if (!QDir(networkManager->getLocalhost()->downloadsFolder).exists())
                QDir().mkdir(networkManager->getLocalhost()->downloadsFolder);
            QString fileName = networkManager->getLocalhost()->downloadsFolder + "/" + fileNameMap[fileID];
            qDebug() << "Saving file on disk as " << fileName;
            QFile file(fileName);
            file.open(QIODevice::WriteOnly);
            file.write(fileContentsMap[fileID]);
            file.close();

            QMessageBox::warning(this->mainWindow, "Download complete", "File saved at " + fileName);
        }

        fileSizeMap.remove(fileID);
        noBlocksMap.remove(fileID);
        currentBlockMap.remove(fileID);
        fileContentsMap.remove(fileID);
        fileNameMap.remove(fileID);
        ownerMap.remove(fileID);
        goesToKVS.remove(fileID);
    } else {
        fileContentsMap[fileID].append(blockContents);
        currentBlockMap[fileID] = currentBlockMap[fileID] + 1;
        QByteArray datagram = Util::serializeVariantMap(Util::createDownloadBlockRequest(fileID, currentBlockMap[fileID]));
        networkManager->sendData(ownerMap[fileID], datagram);
    }
}

void FileManager::receivedUploadNotification(Node from, QString fileID) {
    qDebug() << "Received upload notification from " << from.toString() << " for file " << fileID;
    QString emptyName;
    downloadFile(fileID, from, emptyName, true);
}

void FileManager::receivedKeysMotherfucker(Node from) {
    qDebug() << "Received keys motherfucker!";
    QList<QString> keywords = kvs->keywordToID->keys();
    QList<QString> files = kvs->IDToContents->keys();

    for (int i = 0; i < keywords.size(); i++) {
        QString keywordID = Util::hashName(keywords[i]);
        if (Util::intervalContainsKey(from.getID(), networkManager->getLocalhost()->getID(), keywordID))
            continue;

        for (int j = 0; j < kvs->keywordToID->value(keywords[i]).size(); j++) {
            QPair<QString, QString> currentValue = kvs->keywordToID->value(keywords[i]).at(j);
            QByteArray datagram = Util::serializeVariantMap(Util::createKeywordUpdate(keywords[i], currentValue.first, currentValue.second));
            networkManager->sendData(from, datagram);
        }

        kvs->removeKeyword(keywords[i]);
    }

    for (int i = 0; i < files.size(); i++) {
        if (Util::intervalContainsKey(from.getID(), networkManager->getLocalhost()->getID(), files[i]))
            continue;
        QByteArray datagram = Util::serializeVariantMap(Util::createUploadNotification(files[i]));
        networkManager->sendData(from, datagram);
        removeFromKVS[files[i]] = true;
    }
}

