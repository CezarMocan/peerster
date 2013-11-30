#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>

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

private:
    Ui::MainWindow *ui;    


public slots:
};

#endif // MAINWINDOW_H
