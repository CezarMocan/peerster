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

    // Timer for how often to do rumor-mongering exchange
    static const int ANTI_ENTROPY_FREQ;
    static const int ROUTE_MESSAGE_FREQ;

signals:
    void gotNewMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo);    

public slots:
    // Slot for UI - what to do when Return key is pressed
	void gotReturnPressed();

    // Slot for reading the actual message from the socket;
    // Signal emitted by slot when there's something in the reading queue
    void receiveMessage();

    // Slot for the signal emitted by the anti-entropy timer
    void antiEntropySendStatus();

    // Slot for the signal emitted by the route rumor message timer
    void sendRouteMessage();

    // Slots dealing with adding new peers
    void addNewPeerFromUI();
    void lookedUp(QHostInfo host);

    // Slot called every time a new message is received / generated
    int addReceivedMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo);

private:

    // Localhost info
    Peer *localhost;
    QString localhostName;

    // Network socket for communicating with the other clients
    NetSocket *sock;

    // Port of the last added peer
    QString lastAddedPort;

    // UI Objects
    QLineEdit *addressLine;
    QLabel *addressLabel;
    QLineEdit *portLine;
    QLabel *portLabel;
    QPushButton *addPeerButton;
    QTextEdit *textview;
    MultiLineEdit *textline;
    ReturnKeyFilter *returnKeyFilter;
    QTextEdit *peerview;

    // Messages by Origin name
    QMap<QString, QVector<QString> > messages;

    // List of current peers
    QVector<Peer> peerList;
    QVector<QTimer*> peerTimers;

    // Routing map: name->peer
    QMap<QString, Peer> routingMap;

    // Add peer to peer list and UI list
    void addPeerToList(Peer peer);

    // Called by UI / command line peer adding methods
    void addNewPeerGeneral(QString address, QString portS);

    // Connect with the local instances of peerster
    void discoverPeers();

    // Rumormongering
    void spreadRumor(QString from, QString message, int position);  

    // Print different kinds of maps - debug purposes
    void printMap(QVariantMap map, QString hostName);
    void printRoutingMap(QMap<QString, Peer> map);

    // Main method for parsing a received message and calling what's necessary
    int parseMessage(QByteArray *serializedMessage, QHostAddress sender, quint16 port);
};

#endif // PEERSTER_MAIN_HH
