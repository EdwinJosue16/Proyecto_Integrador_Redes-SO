#ifndef LAYER_LINK
#define LAYER_LINK

#include <thread>
#include <iostream>
#include <map>
#include <cstdint>
#include <string>

#include "CommonTypes.h"
#include "TransmittersController.h"
#include "DgramListener.h"
#include "PackagesDispatcher.h"
#include "Heartbeat.hpp"
#include "fromOtherNodesMiniDispatcher.h"

class LinkLayer
{
private: /// Class members.
	/// Green node id and port.
	green_node_identity_t greenNodeIdentity;

	/// It is used to initialize heartbeat component.
	neighbors_identity_t neighborsIdentity;

	// It is also used to initialize hearbeat component.
	std::map<DestinationId, Transmitter *> redirectingTable;

	// The other optin to avoid these two members is to create the hearbeat component in the green node.

	// Entry point for packages comming from blue-producer or link-layer dispatcher.
	TransmittersController transmittersController;

	/// Component in charge of receiving packages on the socket. That means this is the UDP server.
	DgramListener listener;

	Heartbeat *heartbeat;

public:
	/// Constructor.
	LinkLayer();

	// Destructor.
	~LinkLayer();

public: /// Public interface.
	///
	int start();

	///
	void waitForLinkLayerThreads();

	///
	void receiveIdentity(green_node_identity_t &greenNodeIdentity);

	void receiveNeighborsIdentity(neighbors_identity_t &neighborsIdentity);

	///
	void receiveRedirectingTable(std::map<DestinationId, Transmitter *> &redirectingTable);

	/// It returns a pointer to the inner queue that holds the transmitter-controller; this pointer is then sent to the blue-producer.
	Queue<payload_ptr> *getLinkLayerAcessPoint();

	Queue<payload_ptr> *getHearbeatAccessPoint();

	void setListenerQueue(Queue<payload_ptr> *miniDispatcherQueue);
	
	/// It returns the inner Status Table that hols the Heartbeat component 
	StatusTable* getStatusTableOfNeighbors();

	inline Heartbeat* getHearbeatComponent()
	{
		return this->heartbeat;
	}
};
#endif