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

    connect(mainWindow->connectButton, SIGNAL(clicked()), this, SLOT(connectButtonClicked()));
    connect(this, SIGNAL(connectButtonPushed(Node)), localNode, SLOT(connectButtonPushed(Node)));
}

void Manager::connectButtonClicked() {
    qDebug() << "Button pressed!";
    QHostAddress address(mainWindow->lineEditAddress->text());
    quint16 port = mainWindow->lineEditPort->text().toUInt();
    Node neighbour(address, port);
    emit(connectButtonPushed(neighbour));
}
