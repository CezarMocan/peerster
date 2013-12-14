#include <QList>
#include <QString>
#include <QPair>
#include <QMap>
#include <QDebug>
#include <QObject>

#include "keyvaluestore.h"

KeyValueStore::KeyValueStore(QObject *parent) : QObject(parent) {
    keywordToID = new QMap<QString, QList<QPair<QString, QString> > >();
    IDToContents = new QMap<QString, QByteArray>();
}

quint32 KeyValueStore::BLOCK_SIZE = 8192;

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

    emit(updatedKVS(keywordToID->keys(), IDToContents->keys()));
}

void KeyValueStore::addFile(QString fileID, QByteArray contents) {
    if (IDToContents->find(fileID) == IDToContents->end()) {
        IDToContents->insert(fileID, contents);
        emit(updatedKVS(keywordToID->keys(), IDToContents->keys()));
    }
}

void KeyValueStore::removeFile(QString fileID) {
    IDToContents->remove(fileID);
    emit(updatedKVS(keywordToID->keys(), IDToContents->keys()));
}

void KeyValueStore::removeKeyword(QString keyword) {
    keywordToID->remove(keyword);
    emit(updatedKVS(keywordToID->keys(), IDToContents->keys()));
}


quint32 KeyValueStore::getNoBlocks(QString fileID) {
    int size = (*IDToContents)[fileID].size();
    quint32 result = size / BLOCK_SIZE + (size % BLOCK_SIZE != 0);
    return result;
}

QByteArray KeyValueStore::getBlock(QString fileID, quint32 blockNo) {
    QByteArray file = (*IDToContents)[fileID];
    QByteArray result = file.mid(blockNo * BLOCK_SIZE, BLOCK_SIZE);
    return result;
}


