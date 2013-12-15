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

    keywordToIDReplica = new QMap<QString, QList<QPair<QString, QString> > >();
    IDToContentsReplica = new QMap<QString, QByteArray>();
}

quint32 KeyValueStore::BLOCK_SIZE = 8192;

QList<QPair<QString, QString> > KeyValueStore::keywordLookup(QString keyword) {
    return keywordToID->value(keyword);
}

QList<QPair<QString, QString> > KeyValueStore::keywordLookupReplica(QString keyword) {
    return keywordToIDReplica->value(keyword);
}


QByteArray KeyValueStore::getFile(QString fileID) {
    return IDToContents->value(fileID);
}

QByteArray KeyValueStore::getFileReplica(QString fileID) {
    return IDToContentsReplica->value(fileID);
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

void KeyValueStore::updateKeywordListReplica(QString keyword, QString fileID, QString fileName) {
    qDebug() << "Update keyword list for " << keyword << " " << fileName;
    QPair<QString, QString> pairToUpdate(fileID, fileName);

    QMap<QString, QList<QPair<QString, QString> > >::iterator it = keywordToIDReplica->find(keyword);
    if (it == keywordToIDReplica->end()) {
        QList<QPair<QString, QString> > sampleList;
        sampleList.append(pairToUpdate);
        keywordToIDReplica->insert(keyword, sampleList);
    } else {
        for (int i = 0; i < it.value().size(); i++) {
            if (it.value()[i].first == fileID)
                return;
        }

        (*keywordToIDReplica)[keyword].push_back(pairToUpdate);
    }

    qDebug() << "fileReplica: ";
    for (int i = 0; i < IDToContentsReplica->keys().size(); i++) {
        qDebug() << IDToContentsReplica->keys()[i];
    }
    qDebug() << "keywordToIDReplica: ";
    for (int i = 0; i < keywordToIDReplica->keys().size(); i++) {
        qDebug() << keywordToIDReplica->keys()[i];
    }
}


void KeyValueStore::addFile(QString fileID, QByteArray contents) {
    if (IDToContents->find(fileID) == IDToContents->end()) {
        IDToContents->insert(fileID, contents);
        emit(updatedKVS(keywordToID->keys(), IDToContents->keys()));
    }
}

void KeyValueStore::addFileReplica(QString fileID, QByteArray contents) {
    if (IDToContentsReplica->find(fileID) == IDToContentsReplica->end()) {
        IDToContentsReplica->insert(fileID, contents);
    }

    qDebug() << "fileReplica: ";
    for (int i = 0; i < IDToContentsReplica->keys().size(); i++) {
        qDebug() << IDToContentsReplica->keys()[i];
    }
    qDebug() << "keywordToIDReplica: ";
    for (int i = 0; i < keywordToIDReplica->keys().size(); i++) {
        qDebug() << keywordToIDReplica->keys()[i];
    }
}

void KeyValueStore::removeFile(QString fileID) {
    IDToContents->remove(fileID);
    emit(updatedKVS(keywordToID->keys(), IDToContents->keys()));
}

void KeyValueStore::removeFileReplica(QString fileID) {
    IDToContentsReplica->remove(fileID);
    qDebug() << "fileReplica: ";
    for (int i = 0; i < IDToContentsReplica->keys().size(); i++) {
        qDebug() << IDToContentsReplica->keys()[i];
    }
    qDebug() << "keywordToIDReplica: ";
    for (int i = 0; i < keywordToIDReplica->keys().size(); i++) {
        qDebug() << keywordToIDReplica->keys()[i];
    }
}

void KeyValueStore::removeKeyword(QString keyword) {
    keywordToID->remove(keyword);
    emit(updatedKVS(keywordToID->keys(), IDToContents->keys()));
}

void KeyValueStore::removeKeywordReplica(QString keyword) {
    keywordToIDReplica->remove(keyword);

    qDebug() << "fileReplica: ";
    for (int i = 0; i < IDToContentsReplica->keys().size(); i++) {
        qDebug() << IDToContentsReplica->keys()[i];
    }
    qDebug() << "keywordToIDReplica: ";
    for (int i = 0; i < keywordToIDReplica->keys().size(); i++) {
        qDebug() << keywordToIDReplica->keys()[i];
    }
}

quint32 KeyValueStore::getNoBlocks(QString fileID) {
    int size = (*IDToContents)[fileID].size();
    quint32 result = size / BLOCK_SIZE + (size % BLOCK_SIZE != 0);
    return result;
}

quint32 KeyValueStore::getNoBlocksReplica(QString fileID) {
    int size = (*IDToContentsReplica)[fileID].size();
    quint32 result = size / BLOCK_SIZE + (size % BLOCK_SIZE != 0);
    return result;
}

QByteArray KeyValueStore::getBlock(QString fileID, quint32 blockNo) {
    QByteArray file = (*IDToContents)[fileID];
    QByteArray result = file.mid(blockNo * BLOCK_SIZE, BLOCK_SIZE);
    return result;
}

QByteArray KeyValueStore::getBlockReplica(QString fileID, quint32 blockNo) {
    QByteArray file = (*IDToContentsReplica)[fileID];
    QByteArray result = file.mid(blockNo * BLOCK_SIZE, BLOCK_SIZE);
    return result;
}
