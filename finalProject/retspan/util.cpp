#include <QDebug>
#include <QHostAddress>

#include "util.h"

Util::Util() {
}

QString Util::TYPE = QString("TYPE");
QString Util::NODE_ADDRESS = QString("NODE_ADDRESS");
QString Util::NODE_PORT = QString("NODE_PORT");
QString Util::NODE_ID = QString("NODE_ID");
QString Util::KEY = QString("KEY");
QString Util::POSITION = QString("POSITION");

QString Util::CHORD_QUERY = QString("CHORD_QUERY");
QString Util::CHORD_REPLY = QString("CHORD_REPLY");

QString Util::CHORD_QUERY_PRED = QString("CHORD_QUERY_PRED");
QString Util::CHORD_REPLY_PRED = QString("CHORD_REPLY_PRED");

QString Util::GET_PREDECESSOR = QString("GET_PREDECESSOR");
QString Util::GET_PREDECESSOR_REPLY = QString("GET_PREDECESSOR_REPLY");

QString Util::UPDATE_PREDECESSOR = QString("UPDATE_PREDECESSOR");
QString Util::UPDATE_FINGER = QString("UPDATE_FINGER");

QString Util::KEYWORD_QUERY = QString("KEYWORD_QUERY");
QString Util::KEYWORD_UPDATE = QString("KEYWORD_UPDATE");
QString Util::KEYWORD_REPLY = QString("KEYWORD_REPLY");
QString Util::KEYWORD = QString("KEYWORD");
QString Util::IDS = QString("IDS");
QString Util::NAMES = QString("NAMES");

QString Util::DOWNLOAD_REQUEST = QString("DOWNLOAD_REQUEST");
QString Util::DOWNLOAD_REPLY = QString("DOWNLOAD_REPLY");
QString Util::DOWNLOAD_BLOCK_REQUEST = QString("DOWNLOAD_BLOCK_REQUEST");
QString Util::DOWNLOAD_BLOCK_REPLY = QString("DOWNLOAD_BLOCK_REPLY");

QString Util::FILE_SIZE = QString("FILE_SIZE");
QString Util::NO_BLOCKS = QString("NO_BLOCKS");
QString Util::BLOCK_CONTENTS = QString("BLOCK_CONTENTS");
QString Util::BLOCK_NUMBER = QString("BLOCK_NUMBER");

QString Util::UPLOAD_NOTIFICATION = QString("UPLOAD_NOTIFICATION");
QString Util::KEYS_MOTHERFUCKER = QString("KEYS_MOTHERFUCKER");

QString Util::U_ALIVE_BOSS = QString("U_ALIVE_BOSS");
QString Util::YES_BOSS = QString("YES_BOSS");

QString Util::KEYWORD_UPDATE_REPLICA = QString("KEYWORD_UPDATE_REPLICA");
QString Util::DOWNLOAD_REQUEST_REPLICA = QString("DOWNLOAD_REQUEST_REPLICA");
QString Util::DOWNLOAD_REPLY_REPLICA = QString("DOWNLOAD_REPLY_REPLICA");
QString Util::DOWNLOAD_BLOCK_REQUEST_REPLICA = QString("DOWNLOAD_BLOCK_REQUEST_REPLICA");
QString Util::DOWNLOAD_BLOCK_REPLY_REPLICA = QString("DOWNLOAD_BLOCK_REPLY_REPLICA");
QString Util::UPLOAD_NOTIFICATION_REPLICA = QString("UPLOAD_NOTIFICATION_REPLICA");

/*
QString Util::MAX_VALUE = "ffffffffffffffffffffffffffffffffffffffff";
QString Util::ONE = "0000000000000000000000000000000000000001";
QString Util::ZERO = "0000000000000000000000000000000000000000";
int Util::KEYSPACE_SIZE = 160;
*/

QString Util::MAX_VALUE = "ffff";
QString Util::ONE = "0001";
QString Util::ZERO = "0000";
int Util::KEYSPACE_SIZE = 16;

QString Util::createNodeID(QString name) {

    QByteArray block;
    block.append(name);
    QString result = (QCA::Hash("sha1").hashToString(block));

    return result.left(KEYSPACE_SIZE / 4);
}

QString Util::hashName(QString name) {
    return createNodeID(name);
}

QString Util::hashFileContents(QByteArray fileContents) {
    return QCA::Hash("sha1").hashToString(fileContents).left(KEYSPACE_SIZE / 4);
}

QString Util::normalizeKeyword(QString currentKeyword) {
    QString correctedKeyword;

    for (int j = 0; j < currentKeyword.size(); j++) {
        if (currentKeyword[j] >= 'A' && currentKeyword[j] <= 'Z')
            currentKeyword[j] = currentKeyword[j].toAscii() - 'A' + 'a';
        if ((currentKeyword[j] >= 'a' && currentKeyword[j] <= 'z') || (currentKeyword[j] >= '0' && currentKeyword[j] <= '9'))
            correctedKeyword.append(currentKeyword[j]);
    }

    return correctedKeyword;
}

QString Util::getCircleDifference(QString key1, QString key2) {
    if (key1.size() * 4 != KEYSPACE_SIZE || key2.size() * 4 != KEYSPACE_SIZE) {
        qDebug() << "Key1 size is " << key1.size() << " Key2 size is " << key2.size();
    }

    QString result;
    if (key2 > key1) {
        result = difference(key2, key1);
    } else {
        QString diff = difference(key1, key2);
        int i;
        for (i = diff.size() - 1; i >= 0 && diff[i].toAscii() == '0'; i--);
        if (i == -1)
            diff = MAX_VALUE;
        else
            diff[i] = (getChar(getNumber(diff[i].toAscii()) - 1));
        result = xorMaxValue(diff);
    }

    return result;
}

QString Util::difference(QString key1, QString key2) {
    int i;

    for (i = key2.size() - 1; i >= 0 && key2[i].toAscii() == '0'; i--);
    if (i == -1)
        key2 = MAX_VALUE;
    else {
        key2[i] = (getChar(getNumber(key2[i].toAscii()) - 1));
        for (int j = i + 1; j < key2.size(); j++)
            key2[j] = 'f';
    }

    //qDebug() << "i = " << i << " Difference: key2 after subtracting 1 = " << key2;

    return addition(key1, xorMaxValue(key2));
}

QString Util::addition(QString key1, QString key2) {
    QString sum(key1.size());
    int transport = 0;
    for (int i = key1.size() - 1; i >= 0; i--) {
        int result = getNumber(key1[i].toAscii()) + getNumber(key2[i].toAscii()) + transport;
        sum[i] = QChar(getChar(result % 16));
        transport = result / 16;
    }

    return sum;
}

QString Util::xorMaxValue(QString key) {
    QString result;
    for (int i = 0; i < key.size(); i++) {
        result = result + (getChar(getNumber(key[i].toAscii()) ^ 15));
    }

    return result;
}

char Util::getChar(int number) {
    if (number < 10)
        return '0' + number;
    else
        return 'a' + (number - 10);
}

int Util::getNumber(char character) {
    if (character >= '0' && character <= '9')
        return (character - '0');
    else
        return (character - 'a' + 10);
}

bool Util::intervalContainsKey(QString start, QString stop, QString key) {
    if (start == stop) {
        if (key == start)
            return true;
        else
            return true;
    }
    if (start < stop) {
        if (key >= start && key <= stop)
            return true;
        else
            return false;
    } else {
        if (key >= start && key <= Util::MAX_VALUE)
            return true;
        if (key >= Util::ZERO && key <= stop)
            return true;
        return false;
    }
}

QByteArray Util::serializeVariantMap(QVariantMap map) {
    QByteArray serializedMessage;
    QDataStream serializer(&serializedMessage, QIODevice::WriteOnly);
    serializer << map;
    return serializedMessage;
}

QVariantMap Util::createChordQuery(Node from, QString key) {
    QVariantMap result;
    result.insert(TYPE, QVariant(CHORD_QUERY));
    result.insert(NODE_ADDRESS, QVariant(from.getAddressString()));
    result.insert(NODE_PORT, QVariant(from.getPort()));
    result.insert(NODE_ID, QVariant(from.getID()));
    result.insert(KEY, QVariant(key));

    return result;
}

QVariantMap Util::createChordReply(QString key, Node value) {
    QVariantMap result;
    result.insert(TYPE, QVariant(CHORD_REPLY));
    result.insert(KEY, QVariant(key));
    result.insert(NODE_ADDRESS, QVariant(value.getAddressString()));
    result.insert(NODE_PORT, QVariant(value.getPort()));
    result.insert(NODE_ID, QVariant(value.getID()));

    return result;
}

QVariantMap Util::createChordQueryPred(Node from, QString key) {
    QVariantMap result;
    result.insert(TYPE, QVariant(CHORD_QUERY_PRED));
    result.insert(NODE_ADDRESS, QVariant(from.getAddressString()));
    result.insert(NODE_PORT, QVariant(from.getPort()));
    result.insert(NODE_ID, QVariant(from.getID()));
    result.insert(KEY, QVariant(key));

    return result;
}

QVariantMap Util::createChordReplyPred(QString key, Node value) {
    QVariantMap result;
    result.insert(TYPE, QVariant(CHORD_REPLY_PRED));
    result.insert(KEY, QVariant(key));
    result.insert(NODE_ADDRESS, QVariant(value.getAddressString()));
    result.insert(NODE_PORT, QVariant(value.getPort()));
    result.insert(NODE_ID, QVariant(value.getID()));

    return result;
}


QVariantMap Util::createGetPredecessor(Node from, int position) {
    QVariantMap result;
    result.insert(TYPE, QVariant(GET_PREDECESSOR));
    result.insert(NODE_ADDRESS, QVariant(from.getAddressString()));
    result.insert(NODE_PORT, QVariant(from.getPort()));
    result.insert(NODE_ID, QVariant(from.getID()));
    result.insert(POSITION, QVariant(position));

    return result;
}

QVariantMap Util::createGetPredecessorReply(Node predecessor, int position) {
    QVariantMap result;
    result.insert(TYPE, QVariant(GET_PREDECESSOR_REPLY));
    result.insert(NODE_ADDRESS, QVariant(predecessor.getAddressString()));
    result.insert(NODE_PORT, QVariant(predecessor.getPort()));
    result.insert(NODE_ID, QVariant(predecessor.getID()));
    result.insert(POSITION, QVariant(position));

    return result;
}

QVariantMap Util::createUpdatePredecessor(Node newPredecessor) {
    QVariantMap result;
    result.insert(TYPE, QVariant(UPDATE_PREDECESSOR));
    result.insert(NODE_ADDRESS, QVariant(newPredecessor.getAddressString()));
    result.insert(NODE_PORT, QVariant(newPredecessor.getPort()));
    result.insert(NODE_ID, QVariant(newPredecessor.getID()));

    return result;
}

QVariantMap Util::createUpdateFinger(Node newFinger, int position) {
    QVariantMap result;
    result.insert(TYPE, QVariant(UPDATE_FINGER));
    result.insert(NODE_ADDRESS, QVariant(newFinger.getAddressString()));
    result.insert(NODE_PORT, QVariant(newFinger.getPort()));
    result.insert(NODE_ID, QVariant(newFinger.getID()));
    result.insert(POSITION, QVariant(position));

    return result;

}

QVariantMap Util::createKeywordQuery(QString keyword) {
    QVariantMap result;
    result.insert(TYPE, QVariant(KEYWORD_QUERY));
    result.insert(KEYWORD, QVariant(keyword));

    return result;
}

QVariantMap Util::createKeywordReply(QString keyword, QList<QPair<QString, QString> > results) {
    QVariantMap result;
    QVariantList ids, names;
    result.insert(TYPE, QVariant(KEYWORD_REPLY));
    result.insert(KEYWORD, QVariant(keyword));

    for (int i = 0; i < results.size(); i++) {
        ids.append(QVariant(results[i].first));
        names.append(QVariant(results[i].second));
    }

    result.insert(IDS, ids);
    result.insert(NAMES, names);

    return result;
}

QVariantMap Util::createKeywordUpdate(QString keyword, QString fileID, QString fileName) {
    QVariantMap result;    
    result.insert(TYPE, QVariant(KEYWORD_UPDATE));
    result.insert(KEYWORD, QVariant(keyword));
    result.insert(IDS, QVariant(fileID));
    result.insert(NAMES, QVariant(fileName));

    return result;
}

QVariantMap Util::createDownloadRequest(QString fileID) {
    QVariantMap result;
    result.insert(TYPE, QVariant(DOWNLOAD_REQUEST));
    result.insert(IDS, QVariant(fileID));

    return result;
}

QVariantMap Util::createDownloadReply(QString fileID, quint32 fileSize, quint32 noBlocks) {
    QVariantMap result;
    result.insert(TYPE, QVariant(DOWNLOAD_REPLY));
    result.insert(IDS, QVariant(fileID));
    result.insert(FILE_SIZE, QVariant(fileSize));
    result.insert(NO_BLOCKS, QVariant(noBlocks));

    return result;

}

QVariantMap Util::createDownloadBlockRequest(QString fileID, quint32 block) {
    QVariantMap result;
    result.insert(TYPE, QVariant(DOWNLOAD_BLOCK_REQUEST));
    result.insert(IDS, QVariant(fileID));
    result.insert(BLOCK_NUMBER, QVariant(block));

    return result;
}

QVariantMap Util::createDownloadBlockReply(QString fileID, quint32 block, QByteArray blockContents) {
    QVariantMap result;
    result.insert(TYPE, QVariant(DOWNLOAD_BLOCK_REPLY));
    result.insert(IDS, QVariant(fileID));
    result.insert(BLOCK_NUMBER, QVariant(block));
    result.insert(BLOCK_CONTENTS, QVariant(blockContents));

    return result;
}

QVariantMap Util::createUploadNotification(QString fileID) {
    QVariantMap result;
    result.insert(TYPE, QVariant(UPLOAD_NOTIFICATION));
    result.insert(IDS, QVariant(fileID));

    return result;
}

QVariantMap Util::createKeysMotherfucker() {
    QVariantMap result;
    result.insert(TYPE, QVariant(KEYS_MOTHERFUCKER));

    return result;
}

QVariantMap Util::createUAliveBoss() {
    QVariantMap result;
    result.insert(TYPE, QVariant(U_ALIVE_BOSS));

    return result;
}

QVariantMap Util::createYesBoss() {
    QVariantMap result;
    result.insert(TYPE, QVariant(YES_BOSS));

    return result;
}

QVariantMap Util::createKeywordUpdateReplica(QString keyword, QString fileID, QString fileName) {
    QVariantMap result;
    result.insert(TYPE, QVariant(KEYWORD_UPDATE_REPLICA));
    result.insert(KEYWORD, QVariant(keyword));
    result.insert(IDS, QVariant(fileID));
    result.insert(NAMES, QVariant(fileName));

    return result;
}

QVariantMap Util::createDownloadRequestReplica(QString fileID) {
    QVariantMap result;
    result.insert(TYPE, QVariant(DOWNLOAD_REQUEST_REPLICA));
    result.insert(IDS, QVariant(fileID));

    return result;
}

QVariantMap Util::createDownloadReplyReplica(QString fileID, quint32 fileSize, quint32 noBlocks) {
    QVariantMap result;
    result.insert(TYPE, QVariant(DOWNLOAD_REPLY_REPLICA));
    result.insert(IDS, QVariant(fileID));
    result.insert(FILE_SIZE, QVariant(fileSize));
    result.insert(NO_BLOCKS, QVariant(noBlocks));

    return result;

}

QVariantMap Util::createDownloadBlockRequestReplica(QString fileID, quint32 block) {
    QVariantMap result;
    result.insert(TYPE, QVariant(DOWNLOAD_BLOCK_REQUEST_REPLICA));
    result.insert(IDS, QVariant(fileID));
    result.insert(BLOCK_NUMBER, QVariant(block));

    return result;
}

QVariantMap Util::createDownloadBlockReplyReplica(QString fileID, quint32 block, QByteArray blockContents) {
    QVariantMap result;
    result.insert(TYPE, QVariant(DOWNLOAD_BLOCK_REPLY_REPLICA));
    result.insert(IDS, QVariant(fileID));
    result.insert(BLOCK_NUMBER, QVariant(block));
    result.insert(BLOCK_CONTENTS, QVariant(blockContents));

    return result;
}

QVariantMap Util::createUploadNotificationReplica(QString fileID) {
    QVariantMap result;
    result.insert(TYPE, QVariant(UPLOAD_NOTIFICATION_REPLICA));
    result.insert(IDS, QVariant(fileID));

    return result;
}

void Util::parseChordVariantMap(QVariantMap variantMap, QString &type, Node &node) {
    node.setAddressString(variantMap[NODE_ADDRESS].toString());
    node.setPort(variantMap[NODE_PORT].toUInt());
    node.setID(variantMap[NODE_ID].toString());    
    type = variantMap[TYPE].toString();
}

void Util::parseVariantMapKey(QVariantMap variantMap, QString &key) {
    key = variantMap[KEY].toString();
}

