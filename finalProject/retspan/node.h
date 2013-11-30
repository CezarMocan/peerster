#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QString>
#include <QHostAddress>

class Node : public QObject
{
    Q_OBJECT
public:
    Node();
    Node(const Node& otherNode);
    Node(QHostAddress address, quint16 port);
    Node(QHostAddress address, quint16 port, QString ID);

    QString toString();
    bool operator== (const Node& other) const;
    Node& operator= (Node other);

    QHostAddress getAddress();
    QString getAddressString();
    quint16 getPort();
    QString getID() const;

    void setAddress(QHostAddress address);
    void setAddressString(QString address);
    void setPort(quint16 port);
    void setID(QString ID);

signals:

public slots:

private:
    QHostAddress address;
    quint16 port;
    QString ID;    
};

#endif // NODE_H
