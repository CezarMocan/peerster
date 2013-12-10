#include <QList>
#include <QString>
#include <QPair>
#include <QMap>
#include <QDebug>

#include "keyvaluestore.h"

KeyValueStore::KeyValueStore() {
    keywordToID = new QMap<QString, QList<QPair<QString, QString> > >();
    IDToContents = new QMap<QString, QByteArray>();
}

QList<QPair<QString, QString> > KeyValueStore::keywordLookup(QString keyword) {
    return keywordToID->value(keyword);
}

QByteArray KeyValueStore::getFile(QString fileID) {
    return IDToContents->value(fileID);
}

void KeyValueStore::updateKeywordList(QString keyword, QString fileID, QString fileName) {
    qDebug() << "Update keyword list for " << keyword << " " << fileName;
    QPair<QString, QString> pairToUpdate(fileID, fileName);

    QMap<QString, QList<QPair<QString, QString> > >::iterator it = keywordToID->find(keyword);
    if (it == keywordToID->end()) {
        QList<QPair<QString, QString> > sampleList;
        sampleList.append(pairToUpdate);
        keywordToID->insert(keyword, sampleList);
    } else {
        for (int i = 0; i < it.value().size(); i++) {
            if (it.value()[i].first == fileID)
                return;
        }

        (*keywordToID)[keyword].push_back(pairToUpdate);
    }
}

void KeyValueStore::addFile(QString fileID, QByteArray contents) {
    if (IDToContents->find(fileID) != IDToContents->end()) {
        IDToContents->insert(fileID, contents);
    }
}

