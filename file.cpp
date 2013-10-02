#include <QDebug>
#include <QtCrypto>

#include "file.h"

const int File::BLOCK_SIZE = 8192;

File::File() {
}

File::File(QString fileName) {
    this->fileName = fileName;
    if (parseFile() == 0) {
        qDebug() << "Opened file " << fileName << " with size " << this->fileSize;
        blocklist = hashFile(contents, BLOCK_SIZE);
        qDebug() << "Calculated blocklist";
        fileID = hashFile(blocklist, BLOCK_SIZE);

        qDebug() << blocklist.toHex() << "\n" << fileID.toHex();
    }
}

bool File::operator== (const File& other) const {
    return (fileName == other.fileName);
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
        QByteArray block = contents.left(blockSize);
        qDebug() << "computed block of size " << block.size();
        if (!QCA::isSupported("sha256")) {
            qDebug() << "SHA256 not supported!";
        }

        QCA::Hash shaHash("sha256");
        QByteArray blockHash = shaHash.hash(block).toByteArray();

        result.append(blockHash);
        if (blockSize > contents.size())
            return result;
        contents = contents.mid(blockSize);
    }

    return result;
}
