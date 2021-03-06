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
    static const QString DEFAULT_LAST_IP_KEY;
    static const QString DEFAULT_LAST_PORT_KEY;
    static const QString DEFAULT_BLOCK_REQUEST_KEY;
    static const QString DEFAULT_BLOCK_REPLY_KEY;
    static const QString DEFAULT_DATA_KEY;
    static const QString DEFAULT_SEARCH_KEY;
    static const QString DEFAULT_BUDGET_KEY;
    static const QString DEFAULT_SEARCH_REPLY_KEY;
    static const QString DEFAULT_SEARCH_REPLY_NAMES_KEY;
    static const QString DEFAULT_SEARCH_REPLY_IDS_KEY;


    static const quint32 SEND_PRIVATE;
    static const quint32 RECEIVE_PRIVATE;
    static const quint32 HOP_LIMIT;

    // Default name for receiving a private message from an unknown peer
    static const QString RECEIVED_MESSAGE_WINDOW;

    NetSocket(bool noForwardFlag = 0);

	// Bind this socket to a Peerster-specific default port.
	bool bind();

    QVector<int> getLocalhostPorts();
    int getCurrentPort();

    void sendMessage(QVariantMap status, Peer to);
    void sendMessage(QString from, QString message, int position, QVector<Peer> peerList);
    void sendMessage(QString from, QString message, int position, Peer to, quint32 lastIp, quint16 lastPort);

    void sendPrivateMessage(QString originName, QString peerName, QString message, Peer firstHop, quint32 hopLimit);

    void sendBlockRequest(QString originName, QString dest, QByteArray requestedBlock, Peer firstHop, quint32 hopLimit);
    void sendBlockReply(QString originName, QString dest, QByteArray repliedBlock, QByteArray data, Peer firstHop, quint32 hopLimit);

    void sendSearchRequest(QString originName, QString keyword, quint32 budget, Peer firstHop);
    void sendSearchReply(QString originName, QString dest, quint32 hopLimit, QString keywords, QList<QPair<QString, QByteArray> > searchResults, Peer firstHop);

    void sendVariantMap(QVariantMap variantMap, Peer firstHop);

    void sendStatus(Peer from, QMap<QString, QVector<QString> > messages);

    // Write current message to all ports on localhost
    void writeDatagramPeerList(QByteArray *serializedMessage, QVector<Peer> peerList);
    void writeDatagramSinglePeer(QByteArray *serializedMessage, Peer to);

private:
	int myPortMin, myPortMax;
    int currentPort;
    bool noForwardFlag;

    QVariantMap createStatusMap(QMap<QString, QVector<QString> > messages);
    QVariantMap serializeMessage(QString from, QString text, int position, quint32 lastIp, quint16 lastPort);
    QVariantMap serializePrivateMessage(QString originName, QString peerName, QString message, quint32 hopLimit);
    QVariantMap serializeBlockReply(QString originName, QString dest, QByteArray repliedBlock, QByteArray data, quint32 hopLimit);
    QVariantMap serializeBlockRequest(QString originName, QString dest, QByteArray requestedBlock, quint32 hopLimit);
    QVariantMap serializeSearchRequest(QString originName, QString keyword, quint32 budget);
    QVariantMap serializeSearchReply(QString originName, QString dest, quint32 hopLimit, QString keywords, QList<QPair<QString, QByteArray> > searchResults);

    QByteArray serializeVariantMap(QVariantMap map);

    void printMap(QVariantMap map);
};

#endif
