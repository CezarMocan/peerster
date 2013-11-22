#include "node.h"
#include "util.h"


Node::Node(const Node& otherNode) {
    this->address = otherNode.address;
    this->port = otherNode.port;
}

Node::Node(QHostAddress address, quint16 port) {
    this->address = address;
    this->port = port;
    this->ID = Util::createNodeID(this->toString());
    qDebug() << "Node ID = " << this->ID;
}

QString Node::toString() {
    QString toString = this->address.toString() + ":" + QString::number(this->port);
    return toString;
}

QHostAddress Node::getAddress() {
    return this->address;
}

quint16 Node::getPort() {
    return this->port;
}

QString Node::getID() {
    return this->ID;
}

