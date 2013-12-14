#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "file.h"
#include "util.h"

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
    buttonShareFile = findChild<QPushButton*>("buttonShareFile");
    lineEditSearch = findChild<QLineEdit*>("lineEditSearch");
    buttonSearch = findChild<QPushButton*>("buttonSearch");
    tableWidgetSearchResults = findChild<QTableWidget*>("tableWidgetSearchResults");
    tableWidgetSearchResults->setColumnWidth(0, 300);
    tableWidgetSearchResults->setColumnWidth(1, 100);
    tableWidgetSearchResults->setSelectionBehavior(QAbstractItemView::SelectRows);

    tableWidgetKeyList = findChild<QTableWidget*>("tableWidgetKeyList");
    tableWidgetKeyList->setColumnWidth(0, 250);
    tableWidgetKeyList->setColumnWidth(1, 100);
    tableWidgetKeyList->setSelectionBehavior(QAbstractItemView::SelectRows);

    fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    connect(buttonShareFile, SIGNAL(clicked()), this, SLOT(openFileDialog()));
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

void MainWindow::openFileDialog() {
    QStringList fileNames;
    fileDialog->show();
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
        fileDialog->hide();
        emit(filesOpened(fileNames));
    }
}

void MainWindow::keywordSearchReturned(QVariantList ids, QVariantList names) {
    tableWidgetSearchResults->setRowCount(names.size());

    for (int i = 0; i < names.size(); i++) {
        QTableWidgetItem *name = new QTableWidgetItem(names[i].toString());
        name->setFlags(name->flags() & (~Qt::ItemIsEditable));
        QTableWidgetItem *id = new QTableWidgetItem(ids[i].toString());
        id->setFlags(id->flags() & (~Qt::ItemIsEditable));
        qDebug() << "Keyword search returned with name: " << name;

        tableWidgetSearchResults->setItem(i, 0, name);
        tableWidgetSearchResults->setItem(i, 1, id);
    }
}

void MainWindow::updatedKVS(QList<QString> keywords, QList<QString> files) {
    tableWidgetKeyList->setRowCount(keywords.size() + files.size());
    for (int i = 0; i < keywords.size(); i++) {
        QTableWidgetItem *id = new QTableWidgetItem(keywords[i] + " (" + Util::hashName(keywords[i]) + ")");
        id->setFlags(id->flags() & (~Qt::ItemIsEditable));
        QTableWidgetItem *type = new QTableWidgetItem("keyword");
        type->setFlags(type->flags() & (~Qt::ItemIsEditable));

        tableWidgetKeyList->setItem(i, 0, id);
        tableWidgetKeyList->setItem(i, 1, type);
    }

    for (int i = 0; i < files.size(); i++) {
        QTableWidgetItem *id = new QTableWidgetItem(files[i]);
        id->setFlags(id->flags() & (~Qt::ItemIsEditable));
        QTableWidgetItem *type = new QTableWidgetItem("file");
        type->setFlags(type->flags() & (~Qt::ItemIsEditable));

        tableWidgetKeyList->setItem(i + keywords.size(), 0, id);
        tableWidgetKeyList->setItem(i + keywords.size(), 1, type);
    }
}
