/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *ChordTab;
    QLabel *labelNodeID;
    QTableWidget *tableWidgetFingerTable;
    QLabel *label_2;
    QLabel *label_5;
    QPushButton *connectButton;
    QLabel *label;
    QLabel *labelStatus;
    QLineEdit *lineEditAddress;
    QLabel *label_3;
    QLabel *label_7;
    QLabel *labelPredecessor;
    QLineEdit *lineEditPort;
    QLabel *label_4;
    QLabel *label_6;
    QWidget *tab_2;
    QPushButton *buttonShareFile;
    QWidget *searchTab;
    QPushButton *buttonSearch;
    QLabel *label_8;
    QLineEdit *lineEditSearch;
    QTableWidget *tableWidgetSearchResults;
    QLabel *label_9;
    QMenuBar *menuBar;
    QMenu *menuChord_lookup;
    QMenu *menuSearch;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(455, 524);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 0, 431, 461));
        ChordTab = new QWidget();
        ChordTab->setObjectName(QString::fromUtf8("ChordTab"));
        labelNodeID = new QLabel(ChordTab);
        labelNodeID->setObjectName(QString::fromUtf8("labelNodeID"));
        labelNodeID->setGeometry(QRect(290, 0, 91, 16));
        tableWidgetFingerTable = new QTableWidget(ChordTab);
        if (tableWidgetFingerTable->columnCount() < 3)
            tableWidgetFingerTable->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidgetFingerTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidgetFingerTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidgetFingerTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidgetFingerTable->setObjectName(QString::fromUtf8("tableWidgetFingerTable"));
        tableWidgetFingerTable->setGeometry(QRect(10, 50, 401, 201));
        label_2 = new QLabel(ChordTab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 20, 101, 20));
        label_5 = new QLabel(ChordTab);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(190, 340, 62, 16));
        connectButton = new QPushButton(ChordTab);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));
        connectButton->setGeometry(QRect(0, 370, 114, 41));
        label = new QLabel(ChordTab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(230, 20, 62, 16));
        labelStatus = new QLabel(ChordTab);
        labelStatus->setObjectName(QString::fromUtf8("labelStatus"));
        labelStatus->setGeometry(QRect(290, 20, 81, 16));
        lineEditAddress = new QLineEdit(ChordTab);
        lineEditAddress->setObjectName(QString::fromUtf8("lineEditAddress"));
        lineEditAddress->setGeometry(QRect(70, 340, 113, 20));
        label_3 = new QLabel(ChordTab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 310, 161, 16));
        label_7 = new QLabel(ChordTab);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(230, 0, 62, 16));
        labelPredecessor = new QLabel(ChordTab);
        labelPredecessor->setObjectName(QString::fromUtf8("labelPredecessor"));
        labelPredecessor->setGeometry(QRect(110, 260, 211, 16));
        lineEditPort = new QLineEdit(ChordTab);
        lineEditPort->setObjectName(QString::fromUtf8("lineEditPort"));
        lineEditPort->setGeometry(QRect(230, 340, 113, 22));
        label_4 = new QLabel(ChordTab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 340, 62, 16));
        label_6 = new QLabel(ChordTab);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 260, 81, 16));
        tabWidget->addTab(ChordTab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        buttonShareFile = new QPushButton(tab_2);
        buttonShareFile->setObjectName(QString::fromUtf8("buttonShareFile"));
        buttonShareFile->setGeometry(QRect(150, 370, 114, 32));
        tabWidget->addTab(tab_2, QString());
        searchTab = new QWidget();
        searchTab->setObjectName(QString::fromUtf8("searchTab"));
        buttonSearch = new QPushButton(searchTab);
        buttonSearch->setObjectName(QString::fromUtf8("buttonSearch"));
        buttonSearch->setGeometry(QRect(160, 70, 114, 32));
        label_8 = new QLabel(searchTab);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(190, 10, 62, 16));
        lineEditSearch = new QLineEdit(searchTab);
        lineEditSearch->setObjectName(QString::fromUtf8("lineEditSearch"));
        lineEditSearch->setGeometry(QRect(90, 40, 251, 22));
        tableWidgetSearchResults = new QTableWidget(searchTab);
        if (tableWidgetSearchResults->columnCount() < 2)
            tableWidgetSearchResults->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidgetSearchResults->setHorizontalHeaderItem(0, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidgetSearchResults->setHorizontalHeaderItem(1, __qtablewidgetitem4);
        tableWidgetSearchResults->setObjectName(QString::fromUtf8("tableWidgetSearchResults"));
        tableWidgetSearchResults->setGeometry(QRect(10, 151, 411, 271));
        label_9 = new QLabel(searchTab);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(190, 120, 62, 16));
        tabWidget->addTab(searchTab, QString());
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 455, 22));
        menuChord_lookup = new QMenu(menuBar);
        menuChord_lookup->setObjectName(QString::fromUtf8("menuChord_lookup"));
        menuSearch = new QMenu(menuBar);
        menuSearch->setObjectName(QString::fromUtf8("menuSearch"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuChord_lookup->menuAction());
        menuBar->addAction(menuSearch->menuAction());

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        labelNodeID->setText(QApplication::translate("MainWindow", "Not initialized", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetFingerTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "Start", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetFingerTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "Stop", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidgetFingerTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "Finger", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Finger table", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Port", 0, QApplication::UnicodeUTF8));
        connectButton->setText(QApplication::translate("MainWindow", "Connect!", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Status: ", 0, QApplication::UnicodeUTF8));
        labelStatus->setText(QApplication::translate("MainWindow", "Initializing...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Connect to node in chord", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Node ID:", 0, QApplication::UnicodeUTF8));
        labelPredecessor->setText(QApplication::translate("MainWindow", "Not initialized", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Address", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Predecessor: ", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(ChordTab), QApplication::translate("MainWindow", "Chord info", 0, QApplication::UnicodeUTF8));
        buttonShareFile->setText(QApplication::translate("MainWindow", "Share file!", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "File upload", 0, QApplication::UnicodeUTF8));
        buttonSearch->setText(QApplication::translate("MainWindow", "Search!", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Keyword", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidgetSearchResults->horizontalHeaderItem(0);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidgetSearchResults->horizontalHeaderItem(1);
        ___qtablewidgetitem4->setText(QApplication::translate("MainWindow", "ID", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Results", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(searchTab), QApplication::translate("MainWindow", "Search", 0, QApplication::UnicodeUTF8));
        menuChord_lookup->setTitle(QApplication::translate("MainWindow", "Chord lookup", 0, QApplication::UnicodeUTF8));
        menuSearch->setTitle(QApplication::translate("MainWindow", "Search", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
