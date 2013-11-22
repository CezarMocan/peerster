#include <QDebug>
#include <QHostAddress>

#include "util.h"

Util::Util() {
}

QString Util::MAX_VALUE = "ffffffffffffffffffffffffffffffffffffffff";
int Util::KEYSPACE_SIZE = 160;

QString Util::TYPE = QString("TYPE");
QString Util::NODE_ADDRESS = QString("NODE_ADDRESS");
QString Util::NODE_PORT = QString("NODE_PORT");
QString Util::NODE_ID = QString("NODE_ID");
QString Util::KEY = QString("KEY");

QString Util::CHORD_QUERY = QString("CHORD_QUERY");
QString Util::CHORD_REPLY = QString("CHORD_REPLY");

QString Util::createNodeID(QString name) {
    QByteArray block;
    block.append(name);
    return (QCA::Hash("sha1").hashToString(block));
}

QString Util::getCircleDifference(QString key1, QString key2) {
    if (key1.size() != 40 || key2.size() != 40) {
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
    else
        key2[i] = (getChar(getNumber(key2[i].toAscii()) - 1));

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

void Util::parseChordVariantMap(QVariantMap variantMap, QString &type, Node &node, QString &key) {
    node.setAddressString(variantMap[NODE_ADDRESS].toString());
    node.setPort(variantMap[NODE_PORT].toUInt());
    node.setID(variantMap[NODE_ID].toString());
    key = variantMap[KEY].toString();
    type = variantMap[TYPE].toString();
}

