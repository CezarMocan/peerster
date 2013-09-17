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
#include <QTimer>

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

    void addNewPeerCommandline(QString address);


signals:
    void gotNewMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo);
    void syncedWithPeer(Peer currentPeer);

public slots:
	void gotReturnPressed();
    void receiveMessage();
    void antiEntropySendStatus();
    int addReceivedMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo);
    void addNewPeerFromUI();
    void lookedUp(QHostInfo host);

private:
    QString localStringName;
    QVector<Peer> peerList;
    QVector<QTimer*> peerTimers;
    Peer *localhost;
    QString localhostName;
    NetSocket *sock;
    ReturnKeyFilter *returnKeyFilter;
	QTextEdit *textview;
	MultiLineEdit *textline;
    QString lastAddedPort;

    QLineEdit *addressLine;
    QLabel *addressLabel;

    QLineEdit *portLine;
    QLabel *portLabel;

    QPushButton *addPeerButton;

    QMap<QString, QVector<QString> > messages;
    
    static const int ANTI_ENTROPY_TIMER;


    void discoverPeers();
    void spreadRumor(QString from, QString message, int position);  
    void printMap(QVariantMap map, QString hostName);
    Peer peerLookupByAddress(QHostAddress address, quint16 port);
    int parseMessage(QByteArray *serializedMessage, QHostAddress sender, quint16 port);
};

#endif // PEERSTER_MAIN_HH
