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
#include <QListWidget>

#include "netsocket.hh"
#include "peer.hh"
#include "multilineedit.h"
#include "privatechatdialog.h"

class ChatDialog : public QDialog
{
	Q_OBJECT

public:
	ChatDialog();    

    void addNewPeerCommandline(QString address);

    // Timer for how often to do rumor-mongering exchange
    static const int ANTI_ENTROPY_FREQ;
    static const int ROUTE_MESSAGE_FREQ;
    static const quint32 HOP_LIMIT;
    static const quint32 SEND_PRIVATE;
    static const quint32 RECEIVE_PRIVATE;

    // Localhost info
    Peer *localhost;
    QString localhostName;

signals:
    void gotNewMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo, quint32 hopLimit = 0);
    void receivedPrivateMessage(QString peerName, QString message);

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
    int addReceivedMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo, quint32 hopLimit = 0);

    void peerClicked(QListWidgetItem *item);

private:
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
    QTextEdit *peerview;
    QLabel *peerviewLabel;
    QListWidget *peerNameList;
    QLabel *peerNameListLabel;

    // Messages by Origin name
    QMap<QString, QVector<QString> > messages;

    // List of current peers
    QVector<Peer> peerList;
    QVector<QTimer*> peerTimers;

    // Routing map: name->peer
    QMap<QString, Peer> routingMap;

    // Map of peer names to their corresponding UI private chat windows
    QMap<QString, PrivateChatDialog*> privateChatMap;

    QString RECEIVED_MESSAGE_WINDOW;

    // Create a new window for this peer, if there isn't already one
    void createPrivateDialog(QString peerName);

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
