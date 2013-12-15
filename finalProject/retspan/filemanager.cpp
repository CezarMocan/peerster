#include <QDir>
#include <QFile>
#include <QMessageBox>

#include "filemanager.h"
#include "util.h"
#include "mainwindow.h"

FileManager::FileManager(NetworkManager *networkManager, KeyValueStore *kvs, MainWindow *mainWindow, ChordNode *chordNode, QObject *parent) : QObject(parent) {
    this->networkManager = networkManager;
    connect(this->networkManager, SIGNAL(receivedDownloadRequest(Node,QString)), this, SLOT(receivedDownloadRequest(Node,QString)));
    connect(this->networkManager, SIGNAL(receivedDownloadReply(QString,quint32,quint32)), this, SLOT(receivedDownloadReply(QString,quint32,quint32)));
    connect(this->networkManager, SIGNAL(receivedDownloadBlockRequest(Node,QString,quint32)), this, SLOT(receivedDownloadBlockRequest(Node,QString,quint32)));
    connect(this->networkManager, SIGNAL(receivedDownloadBlockReply(QString,quint32,QByteArray)), this, SLOT(receivedDownloadBlockReply(QString,quint32,QByteArray)));
    connect(this->networkManager, SIGNAL(receivedUploadNotification(Node,QString)), this, SLOT(receivedUploadNotification(Node,QString)));
    connect(this->networkManager, SIGNAL(receivedKeysMotherfucker(Node)), this, SLOT(receivedKeysMotherfucker(Node)));

    connect(this->networkManager, SIGNAL(receivedDownloadRequestReplica(Node,QString)), this, SLOT(receivedDownloadRequestReplica(Node,QString)));
    connect(this->networkManager, SIGNAL(receivedDownloadReplyReplica(QString,quint32,quint32)), this, SLOT(receivedDownloadReplyReplica(QString,quint32,quint32)));
    connect(this->networkManager, SIGNAL(receivedDownloadBlockRequestReplica(Node,QString,quint32)), this, SLOT(receivedDownloadBlockRequestReplica(Node,QString,quint32)));
    connect(this->networkManager, SIGNAL(receivedDownloadBlockReplyReplica(QString,quint32,QByteArray)), this, SLOT(receivedDownloadBlockReplyReplica(QString,quint32,QByteArray)));
    connect(this->networkManager, SIGNAL(receivedUploadNotificationReplica(Node,QString)), this, SLOT(receivedUploadNotificationReplica(Node,QString)));


    this->kvs = kvs;       

    this->mainWindow = mainWindow;
    this->chordNode = chordNode;

    connect(this->chordNode, SIGNAL(updatedPredecessor(Node)), this, SLOT(replicateShitToPredecessor(Node)));
    connect(this->chordNode, SIGNAL(updatedSuccessor(Node)), this, SLOT(updateKeysInSuccessor(Node)));
}

void FileManager::downloadFile(QString fileID, Node owner, QString fileName, bool goToKVS) {
    ownerMap[fileID] = owner;
    fileNameMap[fileID] = fileName;
    goesToKVS[fileID] = goToKVS;
    sentDownloadRequest[fileID] = fileName;

    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadRequest(fileID));
    networkManager->sendData(owner, datagram);
    //qDebug() << "Sending download request for fileID = " << fileID << " to node " << owner.toString();
}

void FileManager::downloadFileReplica(QString fileID, Node owner, QString fileName, bool goToKVS) {
    ownerMapReplica[fileID] = owner;
    fileNameMapReplica[fileID] = fileName;
    goesToKVSReplica[fileID] = goToKVS;
    sentDownloadRequestReplica[fileID] = fileName;

    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadRequestReplica(fileID));
    networkManager->sendData(owner, datagram);
    //qDebug() << "REPLICATION: Sending download request for fileID = " << fileID << " to node " << owner.toString();
}


void FileManager::uploadFile(QString fileID, Node owner) {
//    if (owner == *networkManager->getLocalhost())
//        return;

    removeFromKVS[fileID] = true;
    QByteArray datagram = Util::serializeVariantMap(Util::createUploadNotification(fileID));
    networkManager->sendData(owner, datagram);
    //qDebug() << "Upload file called for " << fileID << " uploading to " << owner.toString();
}

void FileManager::uploadFileReplica(QString fileID, Node owner) {
//    if (owner == *networkManager->getLocalhost())
//        return;

    QByteArray datagram = Util::serializeVariantMap(Util::createUploadNotificationReplica(fileID));
    networkManager->sendData(owner, datagram);
    //qDebug() << "REPLICATION: Upload file called for " << fileID << " uploading to " << owner.toString();
}


void FileManager::receivedDownloadRequest(Node from, QString fileID) {
    if (kvs->getFile(fileID).size() == 0) {
        //qDebug() << "Moving replica to map";
        kvs->addFile(fileID, kvs->getFileReplica(fileID));
        removeFromKVS[fileID] = true;
    }

    quint32 fileSize = kvs->getFile(fileID).size();
    quint32 noBlocks = kvs->getNoBlocks(fileID);
    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadReply(fileID, fileSize, noBlocks));
    networkManager->sendData(from, datagram);
    //qDebug() << "Received download request from " << from.toString() << " for " << fileID << " fileSize = " << fileSize;
}

void FileManager::receivedDownloadReply(QString fileID, quint32 fileSize, quint32 noBlocks) {
    if (fileSize == 0)
        return;

    //qDebug() << "Received download reply for " << fileID << " size = " << fileSize << " noBlocks = " << noBlocks;
    sentDownloadRequest.remove(fileID);

    fileSizeMap[fileID] = fileSize;
    noBlocksMap[fileID] = noBlocks;
    currentBlockMap[fileID] = 0;

    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadBlockRequest(fileID, 0));
    networkManager->sendData(ownerMap[fileID], datagram);
}

void FileManager::receivedDownloadBlockRequest(Node from, QString fileID, quint32 block) {
    //qDebug() << "Received download block request for " << fileID << " block #" << block;
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
    //qDebug() << "Received download block reply for " << fileID << " with block #" << block;
    if (block == noBlocksMap[fileID]) {
        //qDebug() << "Download complete!";
        if (goesToKVS[fileID]) {
            //qDebug() << "Added to KVS";
            kvs->addFile(fileID, fileContentsMap[fileID]);

            //Replication IN PULA MEA
            uploadFileReplica(fileID, chordNode->predecessor);
        } else {
            if (!QDir(networkManager->getLocalhost()->downloadsFolder).exists())
                QDir().mkdir(networkManager->getLocalhost()->downloadsFolder);
            QString fileName = networkManager->getLocalhost()->downloadsFolder + "/" + fileNameMap[fileID];
            //qDebug() << "Saving file on disk as " << fileName;
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
    //qDebug() << "Received upload notification from " << from.toString() << " for file " << fileID;
    QString emptyName;
    downloadFile(fileID, from, emptyName, true);
}

void FileManager::receivedDownloadRequestReplica(Node from, QString fileID) {
    quint32 fileSize = kvs->getFile(fileID).size();
    quint32 noBlocks = kvs->getNoBlocks(fileID);
    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadReplyReplica(fileID, fileSize, noBlocks));
    networkManager->sendData(from, datagram);
    //qDebug() << "REPLICATION: Received download request from " << from.toString() << " for " << fileID;
}

void FileManager::receivedDownloadReplyReplica(QString fileID, quint32 fileSize, quint32 noBlocks) {
    if (fileSize == 0)
        return;

    //qDebug() << "REPLICATION: Received download reply for " << fileID << " size = " << fileSize << " noBlocks = " << noBlocks;
    sentDownloadRequestReplica.remove(fileID);

    fileSizeMapReplica[fileID] = fileSize;
    noBlocksMapReplica[fileID] = noBlocks;
    currentBlockMapReplica[fileID] = 0;

    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadBlockRequestReplica(fileID, 0));
    networkManager->sendData(ownerMapReplica[fileID], datagram);
}

void FileManager::receivedDownloadBlockRequestReplica(Node from, QString fileID, quint32 block) {
    //qDebug() << "REPLICATION: Received download block request for " << fileID << " block #" << block;
    QByteArray blockContents = kvs->getBlock(fileID, block);
    QByteArray datagram = Util::serializeVariantMap(Util::createDownloadBlockReplyReplica(fileID, block, blockContents));
    networkManager->sendData(from, datagram);
    if (block == kvs->getNoBlocks(fileID)) {
        removeFromKVSReplica.remove(fileID);
    }
}

void FileManager::receivedDownloadBlockReplyReplica(QString fileID, quint32 block, QByteArray blockContents) {
//    qDebug() << "REPLICATION: Received download block reply for " << fileID << " with block #" << block;
    if (block == noBlocksMapReplica[fileID]) {
        //qDebug() << "   Download complete!";
        //qDebug() << "   Added to KVS";
        kvs->addFileReplica(fileID, fileContentsMapReplica[fileID]);

        fileSizeMapReplica.remove(fileID);
        noBlocksMapReplica.remove(fileID);
        currentBlockMapReplica.remove(fileID);
        fileContentsMapReplica.remove(fileID);
        fileNameMapReplica.remove(fileID);
        ownerMapReplica.remove(fileID);
        goesToKVSReplica.remove(fileID);
    } else {
        fileContentsMapReplica[fileID].append(blockContents);
        currentBlockMapReplica[fileID] = currentBlockMapReplica[fileID] + 1;
        QByteArray datagram = Util::serializeVariantMap(Util::createDownloadBlockRequestReplica(fileID, currentBlockMapReplica[fileID]));
        networkManager->sendData(ownerMapReplica[fileID], datagram);
    }
}

void FileManager::receivedUploadNotificationReplica(Node from, QString fileID) {
    //qDebug() << "Received upload notification from " << from.toString() << " for file " << fileID;
    QString emptyName;
    downloadFileReplica(fileID, from, emptyName, true);
}

void FileManager::receivedKeysMotherfucker(Node from) {
    //qDebug() << "Received keys motherfucker!";
    QList<QString> keywords = kvs->keywordToID->keys();
    QList<QString> files = kvs->IDToContents->keys();

    //qDebug() << "    have " << files.size() << " files";

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

void FileManager::replicateShitToPredecessor(Node predecessor) {
    QList<QString> keywords = kvs->keywordToID->keys();
    QList<QString> files = kvs->IDToContents->keys();

    for (int i = 0; i < keywords.size(); i++) {
        QString keywordID = Util::hashName(keywords[i]);

        for (int j = 0; j < kvs->keywordToID->value(keywords[i]).size(); j++) {
            QPair<QString, QString> currentValue = kvs->keywordToID->value(keywords[i]).at(j);
            QByteArray datagram = Util::serializeVariantMap(Util::createKeywordUpdateReplica(keywords[i], currentValue.first, currentValue.second));
            networkManager->sendData(predecessor, datagram);
        }
    }

    for (int i = 0; i < files.size(); i++) {
        QByteArray datagram = Util::serializeVariantMap(Util::createUploadNotificationReplica(files[i]));
        networkManager->sendData(predecessor, datagram);
    }
}

void FileManager::updateKeysInSuccessor(Node successor) {
    QList<QString> keywords = kvs->keywordToIDReplica->keys();
    QList<QString> files = kvs->IDToContentsReplica->keys();

    for (int i = 0; i < keywords.size(); i++) {
        QString keywordID = Util::hashName(keywords[i]);

        if (!Util::intervalContainsKey(networkManager->getLocalhost()->getID(), successor.getID(), keywordID))
            continue;

        for (int j = 0; j < kvs->keywordToIDReplica->value(keywords[i]).size(); j++) {
            QPair<QString, QString> currentValue = kvs->keywordToIDReplica->value(keywords[i]).at(j);
            QByteArray datagram = Util::serializeVariantMap(Util::createKeywordUpdate(keywords[i], currentValue.first, currentValue.second));
            networkManager->sendData(successor, datagram);
        }
    }

    for (int i = 0; i < files.size(); i++) {
        if (!Util::intervalContainsKey(networkManager->getLocalhost()->getID(), successor.getID(), files[i]))
            continue;
        QByteArray datagram = Util::serializeVariantMap(Util::createUploadNotification(files[i]));
        kvs->addFile(files[i], kvs->getFileReplica(files[i]));
        removeFromKVS[files[i]] = true;
        //qDebug() << "Sending file " << files[i] << " to " << successor.toString();

        networkManager->sendData(successor, datagram);
    }

    if (!(successor == *networkManager->getLocalhost())) {
        kvs->keywordToIDReplica->clear();
        kvs->IDToContentsReplica->clear();
    }

    QByteArray datagram = Util::serializeVariantMap(Util::createUpdatePredecessor(*networkManager->getLocalhost()));
    networkManager->sendData(successor, datagram);
}


