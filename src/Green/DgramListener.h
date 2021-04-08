#ifndef DGRAM_LISTENER_H
#define DGRAM_LISTENER_H

/*
*	This class is a DGRAM socket wrapper. It was implemented based on "Beej’s Guide to Network Programming" guide made 
*	by Brian Hall.
*	This guide is can be accessed by anyone, and it is available on http://beej.us/guide/bgnet/.
*/

#include <sys/socket.h>
#include <string>

#include "CommonTypes.h"
#include "../common/Producer.hpp"
#include "../common/Payload.hpp"

class DgramListener : public Producer<payload_ptr>
{
private:
	/// Server in which *this listens.
	std::string port;

	/// Socket file descriptor.
	int socketfd;

	std::string localIp;

	/// Buffer to receive packages.
	payload_ptr bufferPackage;

	/// Talker information.
	struct sockaddr_storage their_addr;

	/// Stop condition for listener.
	bool work;

public:
	/// Default constructor.
	DgramListener();

	/// Constructor.
	DgramListener(std::string listenerPort);

	// Destructor.
	~DgramListener();

public:
	/// Do the message production in its own execution thread
	virtual int run() override;

	/// It receives the port number that is needed in order to create the inner socket.
	void receivePort(std::string listenerPort);

private:
	/// It builds a socket for the server and start listening on it.
	int start();

	/// It builds a DGRAM socket a binds *this socket-file-descriptor to it.
	int buildAndBindSocket();

	/// It receives messages from other sockets.
	int listen();

	/// It closes the file descriptor for the socket. No messages are received after this.
	void end();

	/// It returns the ip address of the las client that sent a message to the server.
	std::string getTalkerIp();

	/// It gets sockaddr, IPv4 or IPv6:
	void *get_in_addr(struct sockaddr *sa);

	/// It prints the package content and from who ip address it was received.
	void printPackage(payload_ptr package);

	void getLocalIp();
};
#endif // DGRAM_LISTENER_H