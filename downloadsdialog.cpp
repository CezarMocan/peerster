#include <QDebug>
#include <QTableWidgetItem>

#include "downloadsdialog.h"

DownloadsDialog::DownloadsDialog(QWidget *parent) : QDialog(parent) {

    //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    adjustSize();

    searchResultsView = new QTableWidget(0, 3, this);
    searchResultsView->setMaximumWidth(800);
    searchResultsView->setMinimumWidth(800);

    searchResultsView->setMaximumHeight(400);
    searchResultsView->setMinimumHeight(400);

    searchResultsView->setColumnWidth(0, 400);
    searchResultsView->setColumnWidth(1, 200);
    searchResultsView->setColumnWidth(2, 180);
    searchResultsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    searchResultsView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QTableWidgetItem* fileName = new QTableWidgetItem(QString("File name"), QTableWidgetItem::Type);
    QTableWidgetItem* shaHash = new QTableWidgetItem(QString("SHA256"), QTableWidgetItem::Type);
    QTableWidgetItem* owner = new QTableWidgetItem(QString("Owner"), QTableWidgetItem::Type);
    searchResultsView->setHorizontalHeaderItem(0, fileName);
    searchResultsView->setHorizontalHeaderItem(1, shaHash);
    searchResultsView->setHorizontalHeaderItem(2, owner);

    connect(searchResultsView, SIGNAL(cellDoubleClicked(int,int)),
            this, SLOT(tableCellDoubleClicked(int, int)));

    searchResultsLabel = new QLabel("Search results", this);

    QVBoxLayout *keywordSearchLayout = new QVBoxLayout();
    keywordSearchLayout->addWidget(searchResultsLabel, 1, Qt::AlignCenter);
    keywordSearchLayout->addWidget(searchResultsView, 50, Qt::AlignCenter);
    //searchResultsView->

    setLayout(keywordSearchLayout);
    hide();
}

void DownloadsDialog::append(QString currName, QString currID, QString originName) {
    searchResultsView->setRowCount(searchResultsView->rowCount() + 1);
    int row = searchResultsView->rowCount() - 1;

    QTableWidgetItem *name = new QTableWidgetItem(currName);
    name->setFlags(name->flags() & (~Qt::ItemIsEditable));
    QTableWidgetItem *id = new QTableWidgetItem(currID);
    id->setFlags(id->flags() & (~Qt::ItemIsEditable));
    QTableWidgetItem *origin = new QTableWidgetItem(originName);
    origin->setFlags(origin->flags() & (~Qt::ItemIsEditable));

    searchResultsView->setItem(row, 0, name);
    searchResultsView->setItem(row, 1, id);
    searchResultsView->setItem(row, 2, origin);
}

QList<QTableWidgetItem*> DownloadsDialog::findItems(QString item) {
    return searchResultsView->findItems(item, Qt::MatchExactly);
}

void DownloadsDialog::tableCellDoubleClicked(int row, int column) {
    qDebug() << "Item at (" << row << "," << column << ") clicked";
    QString fileName = searchResultsView->item(row, 0)->text();
    QByteArray shaHash = QByteArray::fromHex(searchResultsView->item(row, 1)->text().toAscii());
    QString owner = searchResultsView->item(row, 2)->text();

    emit(downloadRequest(fileName, shaHash, owner));
}

void DownloadsDialog::showDialog() {
    searchResultsView->clear();
    searchResultsView->setRowCount(0);
    show();
    raise();
}
