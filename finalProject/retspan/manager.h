#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>

#include "chordnode.h"
#include "mainwindow.h"
#include "networkmanager.h"
#include "node.h"
#include "util.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    NetworkManager *chordManager;
    Node *localhost;
    ChordNode *localNode;
    MainWindow *mainWindow;

signals:
    void connectButtonPushed(Node neighbour);

public slots:
    void connectButtonClicked();


};

#endif // MANAGER_H
