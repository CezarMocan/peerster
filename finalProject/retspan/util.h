#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QtCrypto>

class Util
{
public:
    static QString MAX_VALUE;

    Util();
    static QString createNodeID(QString name);
    static QString getCircleDifference(QString key1, QString key2);
    static QString difference(QString key1, QString key2);
    static QString addition(QString key1, QString key2);
    static QString xorMaxValue(QString key);

private:
    static char getChar(int number);
    static int getNumber(char character);
};

#endif // UTIL_H
