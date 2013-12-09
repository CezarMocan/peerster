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
    mainWindow->labelNodeID->setText(localhost->getID());
    mainWindow->show();

    connect(mainWindow->connectButton, SIGNAL(clicked()), this, SLOT(connectButtonClicked()));
    connect(this, SIGNAL(connectButtonPushed(Node)), localNode, SLOT(connectButtonPushed(Node)));
    connect(localNode, SIGNAL(stateUpdateReady()), mainWindow, SLOT(stateUpdateReady()));
    connect(localNode, SIGNAL(stateUpdateUpdatingOthers()), mainWindow, SLOT(stateUpdateUpdatingOthers()));
    connect(localNode, SIGNAL(updatedFingerTable(QVector<FingerEntry>)), mainWindow, SLOT(updatedFingerTable(QVector<FingerEntry>)));
    connect(localNode, SIGNAL(updatedPredecessor(Node)), mainWindow, SLOT(updatedPredecessor(Node)));
}

void Manager::connectButtonClicked() {
    qDebug() << "Button pressed!";
    QHostAddress address(mainWindow->lineEditAddress->text());
    quint16 port = mainWindow->lineEditPort->text().toUInt();
    Node neighbour(address, port);
    emit(connectButtonPushed(neighbour));
}
