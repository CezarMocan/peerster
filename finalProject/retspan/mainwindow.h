#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

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

private:
    Ui::MainWindow *ui;    


public slots:
    void stateUpdateUpdatingOthers();
    void stateUpdateReady();

};

#endif // MAINWINDOW_H
