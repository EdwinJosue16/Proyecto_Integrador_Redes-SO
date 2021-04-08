#include "DgramTalker.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "Log.hpp"

DgramTalker::DgramTalker(std::string targetIp, std::string targetPort)
	: ip{targetIp},
	  port{targetPort}
{
}

DgramTalker::~DgramTalker()
{
	close(this->sockfd);
	// This pointer must be freed after the socket is closed. The reason is that it is used as as class member.
	freeaddrinfo(this->serverInformation);
}

int DgramTalker::start()
{
	if (this->buildSocket() != EXIT_SUCCESS)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

int DgramTalker::sendPackage(payload_ptr package)
{
	auto linkLayerPayload = std::static_pointer_cast<LinkLayerPayload>(package);

	size_t bufferBytesCount = linkLayerPayload->getBytesRepresentationCount();
	char buffer[bufferBytesCount];
	memset(buffer, '\0', bufferBytesCount);

	linkLayerPayload->serialize(buffer);

	Log::append("***** Dgram Talker            ***** < The package has been serialized into a buffer. >");
	Log::append("***** Dgram Talker            ***** < Talker a buffer will be sent through the socket. >");

	int bytes = 0;

	if ((bytes = sendto(this->sockfd,
						buffer,
						bufferBytesCount,
						0,
						this->serverInformation->ai_addr,
						this->serverInformation->ai_addrlen)) == -1)
	{
		Log::append("***** Dgram Talker            ***** < Something went wrong with sendto " + std::string(strerror(errno)) + " >");
	}
	else
	{
		Log::append("***** Dgram Talker            ***** < " + std::to_string(bytes) + " bytes were sent through the socket. >");
	}

	return bytes;
}

int DgramTalker::buildSocket()
{
	Log::append("***** Dgram Talker            ***** < buildSocket started. >");
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;
	int status;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;		// IPv4
	hints.ai_socktype = SOCK_DGRAM; // UDP;

	//std::cout << "***** Dgram Talker            ***** < Talker port: " << this->port << " Talker ip: " << this->ip << "\n";

	if ((status = getaddrinfo(this->ip.c_str(), this->port.c_str(), &hints, &servinfo)) != 0)
	{
		Log::append("***** Dgram Talker            ***** < Error:  getaddrinfo. >");
		return EXIT_FAILURE;
	}

	// Loop through all the results and make a socket.
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((this->sockfd = socket(p->ai_family, p->ai_socktype,
								   p->ai_protocol)) == -1)
		{
			Log::append("***** Dgram Talker            ***** < Error:  socket. >");
			continue;
		}
		break;
	}

	if (p == nullptr)
	{
		Log::append("***** Dgram Talker            ***** < Error: talker: failed to create socket. >");
		return EXIT_FAILURE;
	}

	// It stores the server information on *this.
	this->serverInformation = p;

	Log::append("***** Dgram Talker            ***** < The socket was successfully created. >");
	//std::cout << "***** Dgram Talker            ***** < The socket was successfully created and ready to send messages to the listener.\n";

	return EXIT_SUCCESS;
}

void DgramTalker::printPackage(payload_ptr package)
{
	std::cout << "Package content is:\n";
	std::cout << "Destination ip " << package->getType() << std::endl;
	//   << "Destination port " << package.destPort << std::endl
	//   << "Source ip " << package.sourceIp << std::endl
	//   << "Source ip " << package.sourcePort << std::endl
	//   << "Payload message: " << package.payload.message << "\n\n";
}
