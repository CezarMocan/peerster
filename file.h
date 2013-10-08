#ifndef FILE_H
#define FILE_H

#include <QByteArray>
#include <QString>
#include <QFile>
#include <QMap>

class File
{
public:
    static const int BLOCK_SIZE;
    static const int SHA_SIZE;

    QString fileName;

    File();
    File(QString fileName, QMap<QByteArray, QByteArray> *hashToBlock, QMap<QByteArray, QByteArray> *blockToHash);
    bool operator==(const File& other) const;
    QByteArray getFileID();

private:    
    qint64 fileSize;
    QByteArray blocklist;
    QByteArray fileID;
    QByteArray contents;

    int parseFile();
    QByteArray hashFile(QByteArray contents, int blockSize, QMap<QByteArray, QByteArray> *hashToBlock,
                        QMap<QByteArray, QByteArray> *blockToHash);
};

#endif // FILE_H
