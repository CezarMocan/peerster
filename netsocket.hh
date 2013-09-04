#ifndef NETSOCKET_CLASS_HH
#define NETSOCKET_CLASS_HH

#include <QUdpSocket>

#include "peer.hh"

class NetSocket : public QUdpSocket
{
	Q_OBJECT

public:
	NetSocket();

	// Bind this socket to a Peerster-specific default port.
	bool bind();

    // Write current message to all ports on localhost
    void writeDatagramPeerList(QByteArray *serializedMessage, QVector<Peer> peerList);
    void writeDatagramSinglePeer(QByteArray *serializedMessage, Peer to);
    QVector<int> getLocalhostPorts();
    int getCurrentPort();

private:
	int myPortMin, myPortMax;
    int currentPort;
};

#endif
