#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QString>
#include <QHostAddress>

class Node : public QObject
{
    Q_OBJECT
public:    
    Node(const Node& otherNode);
    Node(QHostAddress address, quint16 port);

    QString toString();
    QHostAddress getAddress();
    quint16 getPort();
    QString getID();

signals:

public slots:

private:
    QHostAddress address;
    quint16 port;
    QString ID;

};

#endif // NODE_H
