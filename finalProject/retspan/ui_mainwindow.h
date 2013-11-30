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
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLineEdit *lineEditSearchKey;
    QPushButton *searchButton;
    QLineEdit *lineEditSearchKeyResult;
    QLabel *label_2;
    QLineEdit *lineEditAddress;
    QLineEdit *lineEditPort;
    QPushButton *connectButton;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QMenuBar *menuBar;
    QMenu *menuChord_lookup;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(355, 454);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        lineEditSearchKey = new QLineEdit(centralWidget);
        lineEditSearchKey->setObjectName(QString::fromUtf8("lineEditSearchKey"));
        lineEditSearchKey->setGeometry(QRect(20, 60, 281, 21));
        searchButton = new QPushButton(centralWidget);
        searchButton->setObjectName(QString::fromUtf8("searchButton"));
        searchButton->setGeometry(QRect(90, 90, 114, 32));
        lineEditSearchKeyResult = new QLineEdit(centralWidget);
        lineEditSearchKeyResult->setObjectName(QString::fromUtf8("lineEditSearchKeyResult"));
        lineEditSearchKeyResult->setGeometry(QRect(70, 140, 281, 22));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 140, 51, 16));
        lineEditAddress = new QLineEdit(centralWidget);
        lineEditAddress->setObjectName(QString::fromUtf8("lineEditAddress"));
        lineEditAddress->setGeometry(QRect(80, 300, 113, 22));
        lineEditPort = new QLineEdit(centralWidget);
        lineEditPort->setObjectName(QString::fromUtf8("lineEditPort"));
        lineEditPort->setGeometry(QRect(80, 340, 113, 22));
        connectButton = new QPushButton(centralWidget);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));
        connectButton->setGeometry(QRect(80, 370, 114, 32));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(60, 270, 161, 16));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 300, 62, 16));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 340, 62, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 355, 22));
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
        searchButton->setText(QApplication::translate("MainWindow", "Search!", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Result:", 0, QApplication::UnicodeUTF8));
        connectButton->setText(QApplication::translate("MainWindow", "Connect!", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Connect to node in chord", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Address", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Port", 0, QApplication::UnicodeUTF8));
        menuChord_lookup->setTitle(QApplication::translate("MainWindow", "Chord lookup", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
