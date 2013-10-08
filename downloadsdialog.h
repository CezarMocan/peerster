#ifndef DOWNLOADSDIALOG_H
#define DOWNLOADSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QVBoxLayout>

class DownloadsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DownloadsDialog(QWidget *parent = 0);
    void append(QString currName, QString currID, QString originName);

signals:
    void downloadRequest(QString fileName, QByteArray shaHash, QString owner);

public slots:
    void showDialog();
    void tableCellDoubleClicked(int row, int column);

private:
    QTableWidget *searchResultsView;
    QLabel *searchResultsLabel;

};

#endif // DOWNLOADSDIALOG_H
