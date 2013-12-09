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
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLineEdit *lineEditAddress;
    QLineEdit *lineEditPort;
    QPushButton *connectButton;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label;
    QLabel *labelStatus;
    QLabel *label_2;
    QTableWidget *tableWidgetFingerTable;
    QLabel *label_6;
    QLabel *labelPredecessor;
    QLabel *label_7;
    QLabel *labelNodeID;
    QMenuBar *menuBar;
    QMenu *menuChord_lookup;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(796, 502);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        lineEditAddress = new QLineEdit(centralWidget);
        lineEditAddress->setObjectName(QString::fromUtf8("lineEditAddress"));
        lineEditAddress->setGeometry(QRect(450, 350, 113, 20));
        lineEditPort = new QLineEdit(centralWidget);
        lineEditPort->setObjectName(QString::fromUtf8("lineEditPort"));
        lineEditPort->setGeometry(QRect(610, 350, 113, 22));
        connectButton = new QPushButton(centralWidget);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));
        connectButton->setGeometry(QRect(380, 380, 114, 41));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(390, 320, 161, 16));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(390, 350, 62, 16));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(570, 350, 62, 16));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(500, 30, 62, 16));
        labelStatus = new QLabel(centralWidget);
        labelStatus->setObjectName(QString::fromUtf8("labelStatus"));
        labelStatus->setGeometry(QRect(550, 30, 81, 16));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(390, 30, 101, 20));
        tableWidgetFingerTable = new QTableWidget(centralWidget);
        if (tableWidgetFingerTable->columnCount() < 3)
            tableWidgetFingerTable->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidgetFingerTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidgetFingerTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidgetFingerTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidgetFingerTable->setObjectName(QString::fromUtf8("tableWidgetFingerTable"));
        tableWidgetFingerTable->setGeometry(QRect(390, 60, 401, 201));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(390, 270, 81, 16));
        labelPredecessor = new QLabel(centralWidget);
        labelPredecessor->setObjectName(QString::fromUtf8("labelPredecessor"));
        labelPredecessor->setGeometry(QRect(490, 270, 211, 16));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(500, 10, 62, 16));
        labelNodeID = new QLabel(centralWidget);
        labelNodeID->setObjectName(QString::fromUtf8("labelNodeID"));
        labelNodeID->setGeometry(QRect(570, 10, 91, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 796, 22));
        menuChord_lookup = new QMenu(menuBar);
        menuChord_lookup->setObjectName(QString::fromUtf8("menuChord_lookup"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuChord_lookup->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        connectButton->setText(QApplication::translate("MainWindow", "Connect!", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Connect to node in chord", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Address", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Port", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Status: ", 0, QApplication::UnicodeUTF8));
        labelStatus->setText(QApplication::translate("MainWindow", "Initializing...", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Finger table", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidgetFingerTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "Start", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidgetFingerTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "Stop", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidgetFingerTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "Finger", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Predecessor: ", 0, QApplication::UnicodeUTF8));
        labelPredecessor->setText(QApplication::translate("MainWindow", "Not initialized", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Node ID:", 0, QApplication::UnicodeUTF8));
        labelNodeID->setText(QApplication::translate("MainWindow", "Not initialized", 0, QApplication::UnicodeUTF8));
        menuChord_lookup->setTitle(QApplication::translate("MainWindow", "Chord lookup", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
