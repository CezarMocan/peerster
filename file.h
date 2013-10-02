#ifndef FILE_H
#define FILE_H

#include <QByteArray>
#include <QString>
#include <QFile>

class File
{
public:
    static const int BLOCK_SIZE;
    File(QString fileName);

private:
    QString fileName;
    qint64 fileSize;
    QByteArray blocklist;
    QByteArray blocklistHash;
    QByteArray contents;

    int parseFile();
    QByteArray hashFile(QByteArray contents, int blockSize);
};

#endif // FILE_H
