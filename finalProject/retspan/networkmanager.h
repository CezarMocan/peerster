#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QUdpSocket>
#include <QObject>

#include "node.h"

class NetworkManager : public QUdpSocket
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = 0);
    bool bind();
    void sendData(Node remotePeer, QByteArray data);
    quint16 getLocalPort();
    Node* getLocalhost();

signals:
    void receivedChordQuery(Node to, QString key);
    void receivedChordReply(QString key, Node value);

public slots:
    void receiveData();

private:
    quint16 myPortMin;
    quint16 myPortMax;
    quint16 currentPort;
    Node *localhost;
};

#endif // NETWORKMANAGER_H
