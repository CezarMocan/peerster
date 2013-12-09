#include "node.h"
#include "util.h"

#include <QHostAddress>

Node::Node() {
    this->address = NULL;
    this->port = NULL;
    this->ID = "";
}

Node::Node(const Node& otherNode) {
    this->address = otherNode.address;
    this->port = otherNode.port;
    this->ID = otherNode.ID;
}

Node::Node(QHostAddress address, quint16 port) {
    this->address = address;
    this->port = port;
    this->ID = Util::createNodeID(this->toString());
    //qDebug() << "Node ID = " << this->ID;
}

Node::Node(QHostAddress address, quint16 port, QString ID) {
    this->address = address;
    this->port = port;
    this->ID = ID;
}

QString Node::toString() {
    QString toString = this->address.toString() + ":" + QString::number(this->port);
    return toString;
}

bool Node::operator== (const Node& other) const {
    return (address == other.address && \
        port == other.port && ID == other.ID);
}

Node& Node::operator= (Node other) {
    this->address = other.address;
    this->port = other.port;
    this->ID = other.ID;
}

QString Node::getAddressString() {
    return this->address.toString();
}

QHostAddress Node::getAddress() {
    return this->address;
}

quint16 Node::getPort() {
    return this->port;
}

QString Node::getID() const {
    return this->ID;
}

void Node::setAddress(QHostAddress address) {
    this->address = address;
}

void Node::setAddressString(QString address) {
    this->address = QHostAddress(address);
}

void Node::setPort(quint16 port) {
    this->port = port;
}

void Node::setID(QString ID) {
    this->ID = ID;
}


