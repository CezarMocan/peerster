#include "keywordtimer.h"

KeywordTimer::KeywordTimer(QObject *parent, QString keyword) :
    QTimer(parent)
{
    this->keyword = keyword;
}

QString KeywordTimer::getKeyword() {
    return this->keyword;
}
