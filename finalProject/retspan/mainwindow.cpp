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
    labelPredecessor = findChild<QLabel*>("labelPredecessor");
    tableWidgetFingerTable = findChild<QTableWidget*>("tableWidgetFingerTable");
    labelNodeID = findChild<QLabel*>("labelNodeID");
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

void MainWindow::updatedFingerTable(QVector<FingerEntry> fingerTable) {
    //tableWidgetFingerTable->setColumnCount(3);
    tableWidgetFingerTable->setColumnWidth(0, 100);
    tableWidgetFingerTable->setColumnWidth(1, 100);
    tableWidgetFingerTable->setColumnWidth(2, 200);
    //tableWidgetFingerTable->setHorizontalHeaderLabels();
    //tableWidgetFingerTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    tableWidgetFingerTable->setRowCount(fingerTable.size());
    for (int i = 0; i < fingerTable.size(); i++) {
        tableWidgetFingerTable->setItem(i, 0, new QTableWidgetItem(fingerTable[i].start));
        tableWidgetFingerTable->setItem(i, 1, new QTableWidgetItem(fingerTable[i].stop));
        tableWidgetFingerTable->setItem(i, 2, new QTableWidgetItem(fingerTable[i].succ.toString()));
    }
}

void MainWindow::updatedPredecessor(Node predecessor) {
    labelPredecessor->setText(predecessor.toString());
}

