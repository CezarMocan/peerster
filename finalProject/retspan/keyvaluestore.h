#ifndef KEYVALUESTORE_H
#define KEYVALUESTORE_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QList>
#include <QPair>
#include <QByteArray>

class KeyValueStore : public QObject
{
    Q_OBJECT

public:   
    explicit KeyValueStore(QObject *parent = 0);

    static quint32 BLOCK_SIZE;

    QList<QPair<QString, QString> > keywordLookup(QString keyword);
    QByteArray getFile(QString fileID);

    void updateKeywordList(QString keyword, QString fileID, QString fileName);
    void addFile(QString fileID, QByteArray contents);

    void removeFile(QString fileID);
    void removeKeyword(QString keyword);

    quint32 getNoBlocks(QString fileID);
    QByteArray getBlock(QString fileID, quint32 blockNo);

    QMap<QString, QList<QPair<QString, QString> > > *keywordToID;
    QMap<QString, QByteArray> *IDToContents;

signals:
    void updatedKVS(QList<QString> keywords, QList<QString> files);

public slots:

private:

};

#endif // KEYVALUESTORE_H
