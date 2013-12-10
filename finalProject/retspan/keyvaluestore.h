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
    KeyValueStore();

    QList<QPair<QString, QString> > keywordLookup(QString keyword);
    QByteArray getFile(QString fileID);

    void updateKeywordList(QString keyword, QString fileID, QString fileName);
    void addFile(QString fileID, QByteArray contents);

private:
    QMap<QString, QList<QPair<QString, QString> > > *keywordToID;
    QMap<QString, QByteArray> *IDToContents;

};

#endif // KEYVALUESTORE_H
