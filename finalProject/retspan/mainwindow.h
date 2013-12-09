#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>

#include "node.h"
#include "fingerentry.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QPushButton *connectButton;
    QLineEdit *lineEditAddress;
    QLineEdit *lineEditPort;
    QLabel *labelStatus;
    QLabel *labelPredecessor;
    QTableWidget *tableWidgetFingerTable;
    QLabel *labelNodeID;

private:
    Ui::MainWindow *ui;    


public slots:
    void stateUpdateUpdatingOthers();
    void stateUpdateReady();
    void updatedFingerTable(QVector<FingerEntry> fingerTable);
    void updatedPredecessor(Node predecessor);

};

#endif // MAINWINDOW_H
