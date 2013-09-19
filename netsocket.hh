#ifndef NETSOCKET_CLASS_HH
#define NETSOCKET_CLASS_HH

#include <QUdpSocket>
#include <QVariantMap>
#include <QByteArray>
#include <QMap>
#include <QString>
#include <QVector>

#include "peer.hh"

class NetSocket : public QUdpSocket
{
	Q_OBJECT

public:

    static const QString DEFAULT_TEXT_KEY;
    static const QString DEFAULT_ORIGIN_KEY;
    static const QString DEFAULT_SEQ_NO_KEY;
    static const QString DEFAULT_WANT_KEY;
    static const QString DEFAULT_DEST_KEY;
    static const QString DEFAULT_HOP_LIMIT_KEY;

    static const quint32 HOP_LIMIT;

    NetSocket(bool noForwardFlag = 0);

	// Bind this socket to a Peerster-specific default port.
	bool bind();

    QVector<int> getLocalhostPorts();
    int getCurrentPort();

    void sendMessage(QVariantMap status, Peer to);
    void sendMessage(QString from, QString message, int position, QVector<Peer> peerList);
    void sendMessage(QString from, QString message, int position, Peer to);

    void sendPrivateMessage(QString originName, QString peerName, QString message, Peer firstHop, quint32 hopLimit);

    void sendStatus(Peer from, QMap<QString, QVector<QString> > messages);

    // Write current message to all ports on localhost
    void writeDatagramPeerList(QByteArray *serializedMessage, QVector<Peer> peerList);
    void writeDatagramSinglePeer(QByteArray *serializedMessage, Peer to);

private:
	int myPortMin, myPortMax;
    int currentPort;
    bool noForwardFlag;

    QVariantMap createStatusMap(QMap<QString, QVector<QString> > messages);
    QVariantMap serializeMessage(QString from, QString text, int position);
    QVariantMap serializePrivateMessage(QString originName, QString peerName, QString message, quint32 hopLimit);
    QByteArray serializeVariantMap(QVariantMap map);    
};

#endif
