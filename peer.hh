#ifndef PEER_CLASS_HH
#define PEER_CLASS_HH

#include <QString>
#include <QHostAddress>

class Peer : public QObject
{
    Q_OBJECT

public:
    Peer();
    Peer(const Peer& peer);
    Peer(QHostAddress hostAddress, quint16 port);

    QString toString();

    QHostAddress hostAddress;
    quint16 port;

    bool operator<(const Peer& other) const;
    bool operator==(const Peer& other) const;
    Peer& operator=(Peer other);
};

#endif
