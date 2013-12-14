#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QtCrypto>
#include <QByteArray>
#include <QVariant>
#include <QVariantMap>

#include "node.h"

class Util
{
public:
    static QString MAX_VALUE;
    static QString ONE;
    static QString ZERO;

    static int KEYSPACE_SIZE;

    static QString TYPE;
    static QString NODE_ADDRESS;
    static QString NODE_PORT;
    static QString NODE_ID;
    static QString KEY;
    static QString POSITION;

    static QString CHORD_QUERY;
    static QString CHORD_REPLY;
    static QString CHORD_QUERY_PRED;
    static QString CHORD_REPLY_PRED;
    static QString GET_PREDECESSOR;
    static QString GET_PREDECESSOR_REPLY;
    static QString UPDATE_PREDECESSOR;
    static QString UPDATE_FINGER;
    static QString KEYWORD_QUERY;
    static QString KEYWORD_UPDATE;
    static QString KEYWORD_REPLY;
    static QString KEYWORD;
    static QString IDS;
    static QString NAMES;

    static QString DOWNLOAD_REQUEST;
    static QString DOWNLOAD_REPLY;
    static QString DOWNLOAD_BLOCK_REQUEST;
    static QString DOWNLOAD_BLOCK_REPLY;

    static QString FILE_SIZE;
    static QString NO_BLOCKS;
    static QString BLOCK_CONTENTS;
    static QString BLOCK_NUMBER;

    static QString UPLOAD_NOTIFICATION;

    static QString KEYS_MOTHERFUCKER;


    Util();
    static QString createNodeID(QString name);
    static QString hashName(QString name);
    static QString hashFileContents(QByteArray fileContents);

    static QString normalizeKeyword(QString keyword);

    static QString getCircleDifference(QString key1, QString key2);
    static QString difference(QString key1, QString key2);
    static QString addition(QString key1, QString key2);
    static QString xorMaxValue(QString key);
    static bool intervalContainsKey(QString start, QString stop, QString key);

    static QByteArray serializeVariantMap(QVariantMap map);

    static QVariantMap createChordQuery(Node from, QString key);
    static QVariantMap createChordReply(QString key, Node value);
    static QVariantMap createChordQueryPred(Node from, QString key);
    static QVariantMap createChordReplyPred(QString key, Node value);
    static QVariantMap createGetPredecessor(Node from, int position);
    static QVariantMap createGetPredecessorReply(Node predecessor, int position);
    static QVariantMap createUpdatePredecessor(Node newPredecessor);
    static QVariantMap createUpdateFinger(Node newFinger, int position);
    static QVariantMap createKeywordQuery(QString keyword);
    static QVariantMap createKeywordReply(QString keyword, QList<QPair<QString, QString> > results);
    static QVariantMap createKeywordUpdate(QString keyword, QString fileID, QString fileName);

    static QVariantMap createDownloadRequest(QString fileID);
    static QVariantMap createDownloadReply(QString fileID, quint32 fileSize, quint32 noBlocks);
    static QVariantMap createDownloadBlockRequest(QString fileID, quint32 block);
    static QVariantMap createDownloadBlockReply(QString fileID, quint32 block, QByteArray blockContents);

    static QVariantMap createUploadNotification(QString fileID);

    static QVariantMap createKeysMotherfucker();

    static void parseChordVariantMap(QVariantMap variantMap, QString &type, Node &node);
    static void parseVariantMapKey(QVariantMap variantMap, QString &key);

private:
    static char getChar(int number);
    static int getNumber(char character);
};

#endif // UTIL_H
