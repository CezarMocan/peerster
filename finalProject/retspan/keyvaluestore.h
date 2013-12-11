#ifndef KEYVALUESTORE_H
#define KEYVALUESTORE_H

#include <QMap>
#include <QString>
#include <QList>
#include <QPair>
#include <QByteArray>

class KeyValueStore
{
public:
    static quint32 BLOCK_SIZE;

    KeyValueStore();

    QList<QPair<QString, QString> > keywordLookup(QString keyword);
    QByteArray getFile(QString fileID);

    void updateKeywordList(QString keyword, QString fileID, QString fileName);
    void addFile(QString fileID, QByteArray contents);

    void removeFile(QString fileID);
    void removeKeyword(QString keyword);

    quint32 getNoBlocks(QString fileID);
    QByteArray getBlock(QString fileID, quint32 blockNo);

private:
    QMap<QString, QList<QPair<QString, QString> > > *keywordToID;
    QMap<QString, QByteArray> *IDToContents;

};

#endif // KEYVALUESTORE_H
