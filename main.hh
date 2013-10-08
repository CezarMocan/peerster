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
#include <QComboBox>

#include "netsocket.hh"
#include "peer.hh"
#include "multilineedit.h"
#include "privatechatdialog.h"
#include "messagehandler.h"
#include "filemanager.h"
#include "downloadsdialog.h"
#include "keywordtimer.h"

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
    static const quint32 BUDGET_LIMIT;
    static const int KEYWORD_SEARCH_FREQ;
    static const int KEYWORD_SEARCH_MAX_RESULTS;

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
    void retrieveFileByID(QByteArray shaHash, QString peerName, Peer firstHop, quint32 hopLimit, QString fileName = "");

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

    // Send reply for block request
    void handleBlockRequest(QString dest, quint32 hopLimit, QString originName, QByteArray requestedBlock);

    // Slot for search by SHA button
    void searchByShaClicked();

    // Slot for search by keyword button
    void searchByKeywordClicked();

    // Slot for FileManager's signal for when a block request is ready to send
    void sendBlockRequest(QByteArray block, QString originName, quint32 blockID);

    // Move file from pending list to shared files list when transfer is complete
    void transferComplete(QByteArray fileID, QString fileName);

    // Periodically called by the timer for sending keyword search requests
    void newKeywordSearchRequest();

    void parseSearchRequest(QString originName, QString keywords, quint32 budget);

    void parseSearchReply(QString originName, QString keywords, QVariantList matchNames, QVariantList matchIDs);

    void downloadRequest(QString fileName, QByteArray shaHash, QString owner);

private:
    // Network socket for communicating with the other clients
    NetSocket *sock;

    // Message parser
    MessageHandler *messageHandler;

    // File manager
    FileManager *fileManager;

    // Port of the last added peer
    QString lastAddedPort;

    // Window for managing downloads
    DownloadsDialog *downloadsDialog;

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

    QListWidget *pendingView;
    QLabel *pendingLabel;
    QLabel *searchBySHA;
    QLineEdit *shaSearchLine;
    QComboBox *nodeToAskLine;
    QLabel *nodeToAskLabel;
    QPushButton *shaSearchButton;

    QLineEdit *keywordSearchLine;
    QPushButton *keywordSearchButton;

    // List of current peers
    QVector<Peer> peerList;
    QVector<QTimer*> peerTimers;

    // Map of peer names to their corresponding UI private chat windows
    QMap<QString, PrivateChatDialog*> privateChatMap;

    // Remember budget and timers for all the search queries
    QMap<QString, quint32> keywordSearchBudget;
    QMap<QString, KeywordTimer*> keywordSearchTimer;
    QMap<QString, quint32> keywordSearchResults;

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

    // Send the keyword search request
    void sendKeywordSearchRequest(QString keyword, quint32 budget);

    // Print different kinds of maps - debug purposes
    void printMap(QVariantMap map, QString hostName);
    void printRoutingMap(QMap<QString, Peer> map);
};

#endif // PEERSTER_MAIN_HH
