#include "mainwindow.h"
#include "chordnode.h"
#include "networkmanager.h"
#include "util.h"
#include "manager.h"

#include <QtCrypto>
#include <QApplication>

int main(int argc, char *argv[])
{
    QCA::Initializer qcainit;
    QApplication a(argc, argv);

    Manager *manager = new Manager();

    return a.exec();
}
