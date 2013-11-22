#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent) {
    chordManager = new NetworkManager();
    if (!chordManager->bind()) {
        qDebug() << "Error in binding!";
        exit(0);
    }

    localhost = chordManager->getLocalhost();
    localNode = new ChordNode(chordManager, localhost);

    mainWindow = new MainWindow();
    mainWindow->show();
}
