#include "LinkLayer.h"
#include "Log.hpp"

LinkLayer::LinkLayer()
{
}

LinkLayer::~LinkLayer()
{
	delete this->heartbeat;
	this->heartbeat = nullptr;
}

int LinkLayer::start()
{
	Log::append("***** Link Layer              ***** < Link layer started. >");
	// It is assumed that receive-identity, receive-redirecting-table, receive-cache-table, and receive-dispatcher-ptr methods were
	// called before this.
	// It initializes the listener execution.

	this->listener.receivePort(std::string(std::to_string(this->greenNodeIdentity.port)));
	Log::append("***** Link Layer              ***** < The port number was sent to the listener. >");

	Log::append("***** Link Layer              ***** < Start listener thread order was sent. >");
	this->listener.startThread();
	Log::append("***** Link Layer              ***** < After run listener.startThread() >");

	Log::append("***** Link Layer              ***** < Start transmitters-controller thread order was sent. >");
	
	/// It creates the heartbeat component.
	this->heartbeat = new Heartbeat(this->neighborsIdentity, this->greenNodeIdentity);

	///
	this->transmittersController.receiveHeartbeatComponent(this->heartbeat);
	
	// It initializes the transmitters-controller execution and all the transmitters.
	this->transmittersController.startThread();

	// Hearbeat must start after all transmitters were created not before. This is because visitors use the transmiters
	// on creation.

	// It gives each visitor it corresponding transmitter queue.
	this->heartbeat->setTransmitterInfoToComponents(this->redirectingTable);

	this->heartbeat->setTransmitterSempahore(this->transmittersController.getTransmittersReadySempahore());

	Log::append("***** Link Layer              ***** < The heartbeat component was created. >");

	this->heartbeat->startThread();
	Log::append("***** Link Layer              ***** < The heartbeat component was started. >");

	return EXIT_SUCCESS;
}

void LinkLayer::receiveIdentity(green_node_identity_t &greenNodeIdentity)
{
	Log::append("***** Link Layer              ***** < The green node identity was received on the link layer. >");
	this->greenNodeIdentity = greenNodeIdentity;
	this->transmittersController.receiveGreenNodeIdentity(greenNodeIdentity);
	Log::append("***** Link Layer              ***** < The green node identity was sent to the transmitters controller. >");
}

void LinkLayer::receiveNeighborsIdentity(neighbors_identity_t &neighborsIdentity)
{
	this->neighborsIdentity = neighborsIdentity;
}
void LinkLayer::receiveRedirectingTable(std::map<DestinationId, Transmitter *> &redirectingTable)
{
	Log::append("***** Link Layer              ***** < The redirecting table was received on the link layer. >");
	this->transmittersController.receiveRedirectingTable(redirectingTable);
	this->redirectingTable = redirectingTable;
	Log::append("***** Link Layer              ***** < The redirecting table was sent to the transmitters controller layer. >");
}

Queue<payload_ptr> *LinkLayer::getLinkLayerAcessPoint()
{
	Log::append("***** Link Layer              ***** < The transmitters controller queue was sent to the green node. >");
	return this->transmittersController.getQueue();
}

Queue<payload_ptr> *LinkLayer::getHearbeatAccessPoint()
{
	Log::append("***** Link Layer              ***** < The heareat access queue was sent to the green node. >");
	return this->heartbeat->getQueue();
}

void LinkLayer::setListenerQueue(Queue<payload_ptr> *miniDispatcherQueue)
{
	this->listener.setQueue(miniDispatcherQueue);
	Log::append("***** Link Layer              ***** < The mini dispatcher was received. >");
}

void LinkLayer::waitForLinkLayerThreads()
{
	// It waits for all threads to finish the simulation after a stop-condition-package was sent.
	this->transmittersController.waitToFinish();
	this->listener.waitToFinish();
	this->heartbeat->waitForHeartbeat();
	Log::append("***** Link Layer              ***** < Link layer execution finished. >");
}

StatusTable* LinkLayer::getStatusTableOfNeighbors()
{
	return this->heartbeat->getStatusTable();
}