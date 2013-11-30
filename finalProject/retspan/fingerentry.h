#ifndef FINGERENTRY_H
#define FINGERENTRY_H

#include <QObject>

#include "node.h"

class FingerEntry {
public:
    FingerEntry();
    Node succ;
    QString start;
    QString stop;


    bool operator== (const FingerEntry& other) const;
    FingerEntry& operator= (FingerEntry other);

signals:

public slots:

};

#endif // FINGERENTRY_H
