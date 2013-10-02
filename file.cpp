#include <QDebug>
#include <QtCrypto>

#include "file.h"

const int File::BLOCK_SIZE = 8192;

File::File(QString fileName) {
    this->fileName = fileName;
    if (parseFile() == 0) {
        qDebug() << "Opened file " << fileName << " with size " << this->fileSize;
        blocklist = hashFile(contents, BLOCK_SIZE);
        qDebug() << "Calculated blocklist";
        blocklistHash = hashFile(blocklist, BLOCK_SIZE);

        qDebug() << blocklist << " " << blocklistHash;
    }
}

int File::parseFile() {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error in reading file";
        return 1;
    }

    this->fileSize = file.size();
    contents = file.readAll();

    return 0;
}

QByteArray File::hashFile(QByteArray contents, int blockSize) {
    QByteArray result;

    while (contents.size() != 0) {
        qDebug() << "pula";
        QByteArray block = contents.left(blockSize);
        qDebug() << "computed block of size " << block.size();
        QCA::Hash shaHash("md1");
        qDebug() << "declared hash";
        shaHash.hash(block);
        //shaHash.update("pula");
        QByteArray blockHash = shaHash.hash(block).toByteArray();
        qDebug() << "created hash";
        result.append(blockHash);
        if (blockSize > contents.size())
            return result;
        contents = contents.mid(blockSize);
    }

    return result;
}
