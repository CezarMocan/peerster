#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QVariantMap>
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QtCrypto>

#include "netsocket.hh"
#include "peer.hh"

class MessageHandler : public QObject
{
    Q_OBJECT

public:
    MessageHandler(NetSocket *sock, QString localhostName);
    void parse(QByteArray *serializedMessage, QHostAddress sender, quint16 senderPort, QMap<QString, QVector<QString> > messages, QMap<QString, Peer> routingMap);

signals:
    void gotNewRumorMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo, quint32 hopLimit = 0);
    void gotNewPrivateMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo, quint32 hopLimit = 0);
    void gotNewBlockRequest(QString dest, quint32 hopLimit, QString originName, QByteArray requestedBlock);
    void gotNewBlockResponse(QString originName, QByteArray repliedBlock, QByteArray data);
    void handlerAddPeerToList(Peer currentPeer);

private:
    NetSocket *sock;
    QString localhostName;

    QMap<QString, QVector<QString> > messages;
    QMap<QString, Peer> routingMap;


    void parseRumorMessage(QVariantMap textVariantMap, Peer currentPeer);
    void parseStatusMessage(QVariantMap textVariantMap, Peer currentPeer);
    void parsePrivateMessage(QVariantMap textVariantMap, Peer currentPeer);
    void parseBlockRequest(QVariantMap textVariantMap, Peer currentPeer);
    void parseBlockResponse(QVariantMap textVariantMap, Peer currentPeer);

    // Check if I have an entry for dest and if hopLimit is greater than 0
    bool checkForward(quint32 hopLimit, QString dest);

    // Chech if SHA256 of data is equal to hash
    bool checkHash(QByteArray data, QByteArray hash);

    void printMap(QVariantMap map);
};

#endif // MESSAGEHANDLER_H
