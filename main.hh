#ifndef PEERSTER_MAIN_HH
#define PEERSTER_MAIN_HH

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHostInfo>

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

    void addNewPeer(QString address, QString port);


signals:
    void gotNewMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo);

public slots:
	void gotReturnPressed();
    void receiveMessage();
    void receivedStatusFromPeer();
    void antiEntropySendStatus();
    int addReceivedMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo);
    void addNewPeerFromUI();
    void lookedUp(QHostInfo host);

private:
    QString localStringName;
    QVector<Peer> peerList;
    Peer *localhost;
    QString localhostName;
    NetSocket *sock;
    ReturnKeyFilter *returnKeyFilter;
	QTextEdit *textview;
	MultiLineEdit *textline;

    QLineEdit *addressLine;
    QLabel *addressLabel;

    QLineEdit *portLine;
    QLabel *portLabel;

    QPushButton *addPeerButton;

    QMap<QString, QVector<QString> > messages;
    

    static const QString DEFAULT_TEXT_KEY;
    static const QString DEFAULT_ORIGIN_KEY;
    static const QString DEFAULT_SEQ_NO_KEY;
    static const QString DEFAULT_WANT_KEY;
    static const int ANTI_ENTROPY_TIMER;


    void discoverPeers();
    void spreadRumor(QString from, QString message, int position);
    void sendStatus(Peer from);

    void printMap(QVariantMap map, QString hostName);
    Peer peerLookupByAddress(QHostAddress address, quint16 port);
    int parseMessage(QByteArray *serializedMessage, QHostAddress sender, quint16 port);
};

#endif // PEERSTER_MAIN_HH
