#ifndef PRIVATECHATDIALOG_H
#define PRIVATECHATDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QWidget>

#include <multilineedit.h>
#include <peer.hh>

class PrivateChatDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PrivateChatDialog(QWidget *parent = 0, QString peerName = "", QString localhostName = "");

private:
    QString peerName;

    // UI Stuff
    QTextEdit *textview;
    MultiLineEdit *textline;
    QPushButton *sendButton;
    ReturnKeyFilter *returnKeyFilter1;

    // Localhost name to display in chat window
    QString localhostName;

signals:
    // Emitted when I send or receive a message;
    // type = 1 for sent;
    // type = 2 for received;
    void privateChatSendMessage(Peer peer, QString peerName, QString message, quint32 type, quint32 hopLimit = 0);

public slots:
    void buttonClicked();
    void addReceivedPrivateMessage(QString peerName, QString message);
};

#endif // PRIVATECHATDIALOG_H
