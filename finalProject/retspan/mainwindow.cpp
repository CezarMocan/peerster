#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectButton = findChild<QPushButton*>("connectButton");
    lineEditAddress = findChild<QLineEdit*>("lineEditAddress");
    lineEditPort = findChild<QLineEdit*>("lineEditPort");
    labelStatus = findChild<QLabel*>("labelStatus");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::stateUpdateUpdatingOthers() {
    labelStatus->setText("Updating other nodes...");
}

void MainWindow::stateUpdateReady() {
    labelStatus->setText("Ready!");
}
