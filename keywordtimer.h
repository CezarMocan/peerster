#ifndef KEYWORDTIMER_H
#define KEYWORDTIMER_H

#include <QTimer>
#include <QString>

class KeywordTimer : public QTimer
{
    Q_OBJECT
public:
    explicit KeywordTimer(QObject *parent = 0, QString keyword = NULL);
    QString getKeyword();

signals:

public slots:

private:
    QString keyword;


};

#endif // KEYWORDTIMER_H
