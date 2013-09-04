#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QString>
#include <QMap>
#include <QVariant>

#include "netsocket.hh"
#include "peer.hh"


class MultiLineEdit : public QTextEdit 
{
    Q_OBJECT

public: 
    MultiLineEdit(QWidget *parent = 0) : QTextEdit(parent) {}

signals:
    void returnSignal();

public slots:
    void returnKeyPressedSlot();
};

class ReturnKeyFilter : public QObject 
{
    Q_OBJECT

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals: 
    void returnKeyPressedSignal();
};


class ChatDialog : public QDialog
{
	Q_OBJECT

public:
	ChatDialog();
    QVariantMap createStatusMap(QMap<QString, QVector<QString> > messages);

    QVariantMap serializeMessage(QString from, QString text, int position);
    QByteArray serializeVariantMap(QVariantMap map);

    void sendMessage(QVariantMap status, Peer to);
    void sendMessage(QString from, QString message, int position);
    void sendMessage(QString from, QString message, int position, Peer to);


public slots:
	void gotReturnPressed();
    void receiveMessage();
    void receivedStatusFromPeer();

private:
    QString localStringName;
    QVector<Peer> peerList;
    Peer *localhost;
    QString localhostName;
    NetSocket *sock;
    ReturnKeyFilter *returnKeyFilter;
	QTextEdit *textview;
	MultiLineEdit *textline;

    QMap<QString, QVector<QString> > messages;
    

    static const QString DEFAULT_TEXT_KEY;
    static const QString DEFAULT_ORIGIN_KEY;
    static const QString DEFAULT_SEQ_NO_KEY;
    static const QString DEFAULT_WANT_KEY;


    void discoverPeers();
    void spreadRumor(QString from, QString message, int position);
    void sendStatus(Peer from);

    int addReceivedMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo);
    void printMap(QVariantMap map, QString hostName);
    Peer peerLookupByAddress(QHostAddress address, quint16 port);
    int parseMessage(QByteArray *serializedMessage, QHostAddress sender, quint16 port);
};

#endif // PEERSTER_MAIN_HH
