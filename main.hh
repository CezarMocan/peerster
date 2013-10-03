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
#include <QFileDialog>

#include "netsocket.hh"
#include "peer.hh"
#include "multilineedit.h"
#include "privatechatdialog.h"
#include "file.h"
#include "messagehandler.h"

class ChatDialog : public QDialog
{
	Q_OBJECT

public:
    ChatDialog(bool noForwardFlag = 0);

    void addNewPeerCommandline(QString address);

    // Timer for how often to do rumor-mongering exchange
    static const int ANTI_ENTROPY_FREQ;
    static const int ROUTE_MESSAGE_FREQ;
    static const quint32 HOP_LIMIT;

    // Localhost info
    Peer *localhost;
    QString localhostName;

    // Messages by Origin name
    QMap<QString, QVector<QString> > messages;

    // Routing map: name->peer
    QMap<QString, Peer> routingMap;

signals:
    void gotNewMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo, quint32 hopLimit = 0);
    void receivedPrivateMessage(QString peerName, QString message);

public slots:
    // Slot for UI - what to do when Return key is pressed
	void gotReturnPressed();

    // Slot for UI - share file button is pressed, open dialog and get the selected files
    void openFileDialog();

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
    int addReceivedMessage(Peer currentPeer, QString peerName, QString text, quint32 seqNo, quint32 isDirect = 0);

    void peerClicked(QListWidgetItem *item);

    // Add peer to peer list and UI list
    void addPeerToList(Peer peer);

private:
    // Network socket for communicating with the other clients
    NetSocket *sock;

    // Message parser
    MessageHandler *messageHandler;

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

    QPushButton *shareFileButton;
    QFileDialog *fileDialog;
    QListWidget *shareView;
    QLabel *shareLabel;

    // List of current peers
    QVector<Peer> peerList;
    QVector<QTimer*> peerTimers;

    // Map of peer names to their corresponding UI private chat windows
    QMap<QString, PrivateChatDialog*> privateChatMap;

    // List of files that are shared by our peerster
    QVector<File> sharedFiles;        

    // no forward option
    bool noForwardFlag;

    // Called by constructor; Creates the whole UI and adds corresponding signals
    void setUpUI();

    // Create a new window for this peer, if there isn't already one
    void createPrivateDialog(QString peerName);

    // Called by UI / command line peer adding methods
    void addNewPeerGeneral(QString address, QString portS);

    // Connect with the local instances of peerster
    void discoverPeers();

    // Rumormongering
    void spreadRumor(Peer previous, QString from, QString message, int position);

    // Print different kinds of maps - debug purposes
    void printMap(QVariantMap map, QString hostName);
    void printRoutingMap(QMap<QString, Peer> map);

    // Main method for parsing a received message and calling what's necessary
    int parseMessage(QByteArray *serializedMessage, QHostAddress sender, quint16 port);
};

#endif // PEERSTER_MAIN_HH
