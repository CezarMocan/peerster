#include "mainwindow.h"
#include "chordnode.h"
#include "networkmanager.h"
#include "util.h"

#include <QtCrypto>
#include <QApplication>

int main(int argc, char *argv[])
{
    QCA::Initializer qcainit;
    QApplication a(argc, argv);

    NetworkManager *chordManager = new NetworkManager();
    if (!chordManager->bind()) {
        qDebug() << "Error in binding!";
        return 0;
    }

    Node *localhost = chordManager->getLocalhost();
    ChordNode *localNode = new ChordNode(chordManager, localhost);

    QString dummy = "84e10099b28f322347f83d3d621386bb7860e139";

    qDebug() << "Difference = " + Util::getCircleDifference(localhost->getID(), dummy);

    MainWindow w;
    w.show();

    return a.exec();
}
