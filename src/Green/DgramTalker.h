#ifndef DGRAM_TALKER_H
#define DGRAM_TALKER_H

/*
*	This class is a DGRAM socket wrapper. It was implemented based on "Beej’s Guide to Network Programming" guide made 
*	by Brian Hall.
*	This guide is can be accessed by anyone, and it is available on http://beej.us/guide/bgnet/.
*/

#include "../common/Payload.hpp"
#include "CommonTypes.h"

#include <sys/socket.h>

class DgramTalker
{
private:
	/// Ip where *this sends packages.
	std::string ip;

	/// Port where *this sends packages.
	std::string port;

	/// Socket file descriptor.
	int sockfd;

	/// Server information.
	struct addrinfo *serverInformation;

public:
	/// Constructor.
	DgramTalker(std::string targetIp, std::string targetPort);

	/// Destructor.
	~DgramTalker();

public:
	/// It makes socket available to send messages to another socket with *this ip and port.
	int start();

	/// It sends a package through the socket associated with *this.
	int sendPackage(payload_ptr package);

private:
	/// It builds a socket to send messages to server.
	int buildSocket();

	void printPackage(payload_ptr package);
};
#endif // DGRAM_TALKER_H