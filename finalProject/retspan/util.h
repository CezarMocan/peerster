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
    static int KEYSPACE_SIZE;

    static QString TYPE;
    static QString NODE_ADDRESS;
    static QString NODE_PORT;
    static QString NODE_ID;
    static QString KEY;

    static QString CHORD_QUERY;
    static QString CHORD_REPLY;


    Util();
    static QString createNodeID(QString name);
    static QString getCircleDifference(QString key1, QString key2);
    static QString difference(QString key1, QString key2);
    static QString addition(QString key1, QString key2);
    static QString xorMaxValue(QString key);

    static QByteArray serializeVariantMap(QVariantMap map);
    static QVariantMap createChordQuery(Node from, QString key);
    static QVariantMap createChordReply(QString key, Node value);

    static void parseChordVariantMap(QVariantMap variantMap, QString &type, Node &node, QString &key);

private:
    static char getChar(int number);
    static int getNumber(char character);
};

#endif // UTIL_H
