#include <QDebug>
#include <QVBoxLayout>

#include "privatechatdialog.h"
#include "main.hh"

PrivateChatDialog::PrivateChatDialog(QWidget *parent, QString peerName, QString localhostName) :
    QDialog(parent)
{
    this->peerName = peerName;
    this->localhostName = localhostName;
    this->setWindowTitle(peerName);

    textview = new QTextEdit(this);
    textview->setReadOnly(true);

    sendButton = new QPushButton("Send", this);
    connect(sendButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    returnKeyFilter1 = new ReturnKeyFilter();

    textline = new MultiLineEdit(this);
    connect(textline, SIGNAL(returnSignal()),
        this, SLOT(buttonClicked()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(textview, 50);
    layout->addWidget(textline, 10);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(20);
    buttonLayout->addWidget(sendButton, 5);
    buttonLayout->addStretch(20);

    layout->addLayout(buttonLayout);

    setLayout(layout);
    show();
}

void PrivateChatDialog::buttonClicked() {
    qDebug() << "got into buttonClicked";
    QString message = textline->toPlainText();
    textview->append("[" + localhostName + "]: " + message);
    textline->clear();

    emit privateChatSendMessage(*(new Peer()), peerName, message, NetSocket::SEND_PRIVATE, 0); // figure out the empty peer
}

void PrivateChatDialog::addReceivedPrivateMessage(QString peerName, QString message) {
    if (peerName != this->peerName) {
        qDebug() << "PrivateChatDialog: peer names do not match" << peerName << this->peerName;
        return;
    }
    textview->append("[" + peerName + "]: " + message);
    this->show();
    this->setFocus();
}

