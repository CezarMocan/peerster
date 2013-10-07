#include <QtCrypto>
#include <QFile>
#include <QDir>

#include "filemanager.h"

FileManager::FileManager(NetSocket* sock, QString localhostName) {
    this->sock = sock;
    this->localhostName = localhostName;
    this->noReceivedFiles = 0;
}

void FileManager::addFile(QString fileName) {
    File file(fileName, &hashToBlock, &blockToHash);
    sharedFiles.append(file);
}

QByteArray FileManager::getBlockByHash(QByteArray blockHash) {
    return hashToBlock[blockHash];
}

void FileManager::retrieveFile(QByteArray fileID, QString peerName, Peer firstHop, quint32 hopLimit) {
    blocksReceived[fileID].resize(300); // TODO: add constant for this
    qDebug() << "Sending request for hash " << fileID.toHex() << "to peer " << peerName;
    sock->sendBlockRequest(localhostName, peerName, fileID, firstHop, hopLimit);
}

void FileManager::gotNewBlockResponse(QString originName, QByteArray repliedBlock, QByteArray data) {
    QByteArray fileID = getFileID(repliedBlock);
    if (!checkSHA(repliedBlock, data)) {
        qDebug() << "SHA256 of received Data does not match BlockReply";
        return;
    }

    qDebug() << "BLOCK REPLY: " << repliedBlock.toHex();


    hashToBlock.insert(repliedBlock, data);
    blockToHash.insert(data, repliedBlock);

    if (repliedBlock == fileID) {
        idToBlocklist.insert(fileID, data);
        fileBlockSize[fileID] = parseBlocklist(data, originName, fileID);
        qDebug() << "fileBlockSize is " << fileBlockSize[fileID];
        sendRequest(originName, 0, fileID);
    } else {
        qDebug() << "PULAPULAPULA!!!";

        if (!blockNumber.contains(repliedBlock)) {
            qDebug() << "Received a block for which I did not send a request!";
            return;
        }

        int position = blockNumber[repliedBlock];
        qDebug() << "Position is:" << position;
        qDebug() << "File ID is: " << fileID.toHex();

        blocksReceived[fileID][position] = data;
        fileBlockSize[fileID]--;
        qDebug() << "fileBlockSize for current file is: " << fileBlockSize[fileID];

        if (fileBlockSize[fileID] == 0) {
            completeTransfer(fileID);
            return;
        }

        sendRequest(originName, position + 1, fileID);

    }
}

void FileManager::sendRequest(QString originName, int blockNo, QByteArray fileID) {
    QByteArray block = idToBlocklistArray[fileID][blockNo];
    emit blockReadyForSending(block, originName, blockNo);
}

void FileManager::completeTransfer(QByteArray fileID) {
    QByteArray totalFile;
    for (int pos = 0; !blocksReceived[fileID][pos].isEmpty(); pos++) {
        totalFile.append(blocksReceived[fileID][pos]);
    }

    noReceivedFiles++;

    qDebug() << "WOOOO Completely received file" << noReceivedFiles;

    QString fileName = QDir::currentPath() + "/" + localhostName + "received_file_" + QString::number(noReceivedFiles);

    QFile newFile(fileName);
    newFile.open(QIODevice::WriteOnly);
    newFile.write(totalFile);
    newFile.close();    

    emit completedTransfer(fileID, fileName);
}

int FileManager::parseBlocklist(QByteArray blocklist, QString originName, QByteArray fileID) {
    int blockNo = 0;
    QByteArray initialBlocklist = blocklist;
    while (blocklist.size() != 0) {
        QByteArray block = blocklist.left(File::SHA_SIZE);
        belongsTo[block] = initialBlocklist;

        blockNumber[block] = blockNo;
        idToBlocklistArray[fileID].push_back(block);

        blocklist = blocklist.mid(File::SHA_SIZE);
        //emit blockReadyForSending(block, originName, blockNo);
        blockNo++;
    }

    return blockNo;
}

QByteArray FileManager::getFileID(QByteArray block) {
    if (belongsTo.contains(block))
        block = belongsTo[block];

    while (blockToHash.contains(block)) {
        block = blockToHash[block];
    }
    return block;
}

bool FileManager::checkSHA(QByteArray hash, QByteArray data) {
    QCA::Hash shaHash("sha256");
    QByteArray blockHash = shaHash.hash(data).toByteArray();
    return (blockHash == hash);
}
