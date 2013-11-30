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
    static QString GET_PREDECESSOR;
    static QString GET_PREDECESSOR_REPLY;
    static QString UPDATE_PREDECESSOR;
    static QString UPDATE_FINGER;


    Util();
    static QString createNodeID(QString name);
    static QString getCircleDifference(QString key1, QString key2);
    static QString difference(QString key1, QString key2);
    static QString addition(QString key1, QString key2);
    static QString xorMaxValue(QString key);
    static bool intervalContainsKey(QString start, QString stop, QString key);

    static QByteArray serializeVariantMap(QVariantMap map);

    static QVariantMap createChordQuery(Node from, QString key);
    static QVariantMap createChordReply(QString key, Node value);
    static QVariantMap createGetPredecessor(Node from, int position);
    static QVariantMap createGetPredecessorReply(Node predecessor, int position);
    static QVariantMap createUpdatePredecessor(Node newPredecessor);
    static QVariantMap createUpdateFinger(Node newFinger, int position);

    static void parseChordVariantMap(QVariantMap variantMap, QString &type, Node &node);
    static void parseVariantMapKey(QVariantMap variantMap, QString &key);

private:
    static char getChar(int number);
    static int getNumber(char character);
};

#endif // UTIL_H
