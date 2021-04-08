#include "DgramListener.h"
#include "Log.hpp"
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <stdio.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <ifaddrs.h>
#include "./Payloads/LinkLayerPayload.hpp"
#include "./Payloads/HeartbeatPayload.hpp"
#include "./Payloads/BroadcastPayload.h"
#include "./Payloads/SpanningTreePayload.h"
#include "./Payloads/BroadcastReachabilityPayload.h"

DgramListener::DgramListener(std::string listenerPort)
	: port{listenerPort},
	  work{true}
{
}

DgramListener::DgramListener()
	: work{true}

{
}

DgramListener::~DgramListener()
{
}

int DgramListener::run()
{
	return this->start();
}

int DgramListener::start()
{
	if (this->buildAndBindSocket() != EXIT_SUCCESS)
		return EXIT_FAILURE;

	return this->listen();
}

int DgramListener::buildAndBindSocket()
{
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo *p;
	int status;

	memset(&(hints), 0, sizeof(hints));
	hints.ai_family = AF_INET;		// IPv4
	hints.ai_socktype = SOCK_DGRAM; // UDP
	hints.ai_flags = AI_PASSIVE;	// Use host ip.

	if ((status = getaddrinfo(NULL, this->port.c_str(), &(hints), &(servinfo)) != 0))
	{
		Log::append("***** Dgram Listener          ***** <  Error: getaddrinfo with status: " + std::to_string(status) + " >");
		return EXIT_FAILURE;
	}

	// Loop through all the results and bind to the first we can.
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((this->socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			Log::append("***** Dgram Listener          ***** <  Error: socket. >");
			continue;
		}

		if (bind(this->socketfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(this->socketfd);
			Log::append("***** Dgram Listener          ***** <  Error: bind. >");
			continue;
		}
		break;
	}

	if (p == nullptr)
	{
		Log::append("***** Dgram Listener          ***** <  Error: failed to bind socket. >");
		return EXIT_FAILURE;
	}

	freeaddrinfo(servinfo);

	Log::append("***** Dgram Listener          ***** <  Socket was built and binded correctly. >");

	// Socket was built and binded correctly.
	return EXIT_SUCCESS;
}

int DgramListener::listen()
{
	socklen_t addr_len = sizeof(their_addr);
	int bytes = 0;
	char buffer[MAX_BUFFER_SIZE];
	std::memset(buffer, '\0', MAX_BUFFER_SIZE);

	Log::append("***** Dgram Listener          ***** < : Listener is listen (before working) >");

	while (work)
	{
		Log::append("***** Dgram Listener          ***** < : Listener is working >");

		bytes = recvfrom(this->socketfd,
						 buffer,
						 sizeof(buffer),
						 0,
						 (struct sockaddr *)&(this->their_addr),
						 &addr_len);

		if (bytes <= 0)
		{
			Log::append("***** Dgram Listener          ***** < : Error in received message... >");
		}
		else
		{
			Log::append("***** Dgram Listener          ***** < " + std::to_string(bytes) + " bytes were received on the socket. >");
		}

		auto linkLayerPayload = std::make_shared<LinkLayerPayload>();
		linkLayerPayload->deserialize(buffer);

		Log::append("***** Dgram Listener          ***** < : The buffer was deserialized into a payload object. >");

		// ***** It gets the final destination id from link layer payload and set it to network and forwarding payloads *****
		// This is needed for our inner implementation structure because the forwarding frame doesn't store the final destination.
		if (linkLayerPayload->type == LinkLayerPayload::NETWORK_TYPE)
		{
			// The final destination id only exits in the link layer payload. But it must be preserved to be used in the case of a forwarding payload redirection.
			// So this line basically stores the value to reuse it and build a new link layer payload in the case of a needed forward.
			auto networkPayload = std::static_pointer_cast<NetworkPayload>(linkLayerPayload->payload);
			networkPayload->finalDestination = linkLayerPayload->finalDestinationId;

			if (networkPayload->type == NetworkPayload::FORWARDING_TYPE)
			{
				// The forwarding payload need to know the final destination id. This is used in the receiving forwarding module to determine
				// if the target node was reached.
				auto forwardingPayload = std::static_pointer_cast<ForwardingPayload>(networkPayload->payload);
				forwardingPayload->destinationId = networkPayload->finalDestination;
			}
			else if (networkPayload->type == NetworkPayload::BROADCAST_TYPE)
			{
				// In the broadcasting process the message must be redirected to all neighbors except the one who send the message.
				// This is why the immediate source is stored in the network payload.

				auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(networkPayload->payload);
				broadcastPayload->immediateSource = linkLayerPayload->sourceId;

				// This payload will be processed in the broadcast receiving side module.
				broadcastPayload->broadcastSide = BroadcastPayload::BROADCAST_RECEIVING_SIDE;
				
				if (broadcastPayload->type == BroadcastPayload::REACHABILITY_TYPE){

					auto broadcastReachabilityPayload = std::static_pointer_cast<BroadcastReachabilityPayload>(broadcastPayload->payload);
					broadcastReachabilityPayload->broadcastSourceId  = broadcastPayload->sourceId;
				}

			}
			else if (networkPayload->type == NetworkPayload::SPANNING_TREE_TYPE)
			{
				// The order is not swapped here, that is a pink node's job.
				// This is used to recover the sourceId and destination id from the link layer payload because both are needed in the spanning tree payload for the pink node.
				auto spanningTreePayload = std::static_pointer_cast<SpanningTreePayload>(networkPayload->payload);

				spanningTreePayload->sourceId = linkLayerPayload->sourceId;
				spanningTreePayload->destinationId = linkLayerPayload->finalDestinationId;
			}

			Log::append("***** Dgram Listener          ***** < After deserializing the buffer it was possible to determine that the payload type is NETWORK. >");
		}
		else if (linkLayerPayload->type == LinkLayerPayload::HEARTBEAT_TYPE)
		{
			Log::append("***** Dgram Listener          ***** < After deserializing the buffer it was possible to determine that the payload type is HEARTBEAT. >");
		}

		this->produce(linkLayerPayload);

		Log::append("***** Dgram Listener          ***** < The package was sent to dispatcher reception queue. >");

		std::memset(buffer, '\0', MAX_BUFFER_SIZE);
	}

	Log::append("***** Dgram Listener          ***** <  Listener has finished. >");
	return EXIT_SUCCESS;
}

void DgramListener::end()
{
	this->work = false;
	close(this->socketfd);
}

std::string DgramListener::getTalkerIp()
{
	char ip[INET_ADDRSTRLEN];

	return std::string(inet_ntop(this->their_addr.ss_family,
								 this->get_in_addr((struct sockaddr *)&(this->their_addr)),
								 ip, sizeof(ip)));
}

void *DgramListener::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void DgramListener::printPackage(payload_ptr package)
{
	std::cout << "A package was received from " << this->getTalkerIp() << "\n";

	std::cout << "Package content is:\n";
	std::cout << "Destination ip " << package->getType() << std::endl
			  << "Destination port " << package->getType() << std::endl;
	// << "Source ip " << package.sourceIp << std::endl
	// << "Source port " << package.sourcePort << std::endl
	// << "Payload message: " << package.payload.message << "\n\n";
}

void DgramListener::receivePort(std::string listenerPort)
{
	this->port = listenerPort;
}

void DgramListener::getLocalIp()
{
	struct ifaddrs *myaddrs, *ifa;
	void *in_addr;
	char buf[64];

	if (getifaddrs(&myaddrs) != 0)
	{
		perror("getifaddrs");
		exit(1);
	}

	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;
		if (!(ifa->ifa_flags & IFF_UP))
			continue;

		switch (ifa->ifa_addr->sa_family)
		{
		case AF_INET:
		{
			struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
			in_addr = &s4->sin_addr;
			break;
		}

		case AF_INET6:
		{
			struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
			in_addr = &s6->sin6_addr;
			break;
		}

		default:
			continue;
		}

		if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf)))
		{
			printf("%s: inet_ntop failed!\n", ifa->ifa_name);
		}
		else
		{
			printf("%s: %s\n", ifa->ifa_name, buf);
		}
	}

	freeifaddrs(myaddrs);
}
