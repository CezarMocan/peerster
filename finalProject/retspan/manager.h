#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QString>
#include <QPair>

#include "chordnode.h"
#include "mainwindow.h"
#include "networkmanager.h"
#include "node.h"
#include "util.h"
#include "keyvaluestore.h"
#include "filemanager.h"
#include "file.h"


class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    NetworkManager *chordManager;
    Node *localhost;
    ChordNode *localNode;
    MainWindow *mainWindow;
    KeyValueStore *kvs;
    FileManager *fileManager;

signals:
    void connectButtonPushed(Node neighbour);
    void keywordSearchReturned(QVariantList ids, QVariantList names);

public slots:
    void connectButtonClicked();
    void searchButtonClicked();
    void filesOpened(QStringList fileNames);
    void receivedReplyFromChord(QString key, Node node);
    void receivedKeywordQuery(Node from, QString keyword);
    void receivedKeywordReply(QString keyword, QVariantList ids, QVariantList names);
    void receivedKeywordUpdate(QString keyword, QString fileID, QString fileName);
    void gotDownloadRequest(int row, int column);

    void receivedKeywordUpdateReplica(QString keyword, QString fileID, QString fileName);

private:
    QMap<QString, bool> pendingFileQueries;

    QMultiMap<QString, QPair<QString, QPair<QString, QString> > > pendingQueries;
    QMultiMap<QString, QString> pendingKeywordQueries;
    QMultiMap<QString, QString> pendingKeywordResponses;
    QMultiMap<QString, QString> pendingDownloadRequest;

};

#endif // MANAGER_H
