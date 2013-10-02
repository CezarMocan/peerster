#ifndef FILE_H
#define FILE_H

#include <QByteArray>
#include <QString>
#include <QFile>

class File
{
public:
    static const int BLOCK_SIZE;

    QString fileName;

    File();
    File(QString fileName);
    bool operator==(const File& other) const;

private:    
    qint64 fileSize;
    QByteArray blocklist;
    QByteArray fileID;
    QByteArray contents;

    int parseFile();
    QByteArray hashFile(QByteArray contents, int blockSize);
};

#endif // FILE_H
