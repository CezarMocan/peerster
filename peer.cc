#include <QString>

#include "peer.hh"

Peer::Peer() {
    this->hostAddress = NULL;
    this->port = 0;
}

Peer::Peer(const Peer& peer) {
    this->hostAddress = peer.hostAddress;
    this->port = peer.port;
}

Peer::Peer(QHostAddress hostAddress, quint16 port) {
    this->hostAddress = hostAddress;
    this->port = port;
}

bool Peer::operator< (const Peer& other) const {
    return (port < other.port);
}

QString Peer::toString() {
    QString stringPeer = hostAddress.toString() + ":" + QString::number(port);
    return stringPeer;
}

bool Peer::operator== (const Peer& other) const {
    return (hostAddress == other.hostAddress && \
        port == other.port);
}

Peer& Peer::operator= (Peer other) {
    this->hostAddress = other.hostAddress;
    this->port = other.port;
}

