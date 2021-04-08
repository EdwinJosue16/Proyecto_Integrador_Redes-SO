#include "NetworkLayer.h"

NetworkLayer::NetworkLayer()
{
	Log::append("***** Network Layer           ***** < It was created. >");
}

NetworkLayer::~NetworkLayer()
{
	Log::append("***** Network Layer           ***** < It was destroyed. >");
}

int NetworkLayer::start()
{
	this->fromApplicationLayerDispatcher.registerRedirect(FromApplicationLayerDispatcher::FORWARDING_QUEUE,
														  this->forwardingSendingSide.getQueue());

	this->fromApplicationLayerDispatcher.registerRedirect(FromApplicationLayerDispatcher::BROADCAST_QUEUE,
														  this->broadcastSendingSide.getQueue());

	this->fromLinkLayerDispatcher.registerRedirect(FORWARDING_QUEUE,
												   this->forwardingReceivingSide.getQueue());

	this->fromLinkLayerDispatcher.registerRedirect(BROADCAST_QUEUE,
												   this->broadcastReceivingSide.getQueue());

	this->fromLinkLayerDispatcher.registerRedirect(SPANNING_TREE_QUEUE,
												   this->spanningTreeReceivingSide.getQueue());

	this->fromPinkAgentDispatcher.registerRedirect(FromPinkAgentDispatcher::FORWARDING_QUEUE,
												   this->forwardingUpdateReceiver.getQueue());

	this->fromPinkAgentDispatcher.registerRedirect(FromPinkAgentDispatcher::BROADCAST_QUEUE,
												   this->neighborsInSpanningTreeUpdateReceiver.getQueue());

	this->fromPinkAgentDispatcher.registerRedirect(FromPinkAgentDispatcher::SPANNING_TREE_QUEUE,
												   this->spanningTreeSendingSide.getQueue());

	this->neighborsInSpanningTreeUpdateReceiver.setBroadcaster(&(this->broadcaster));

	this->forwardingUpdateReceiver.setForwardingComponents(&(this->forwardingSendingSide), &(this->forwardingReceivingSide));

	this->broadcastSendingSide.setBroadcaster(&(this->broadcaster));
	this->broadcastReceivingSide.setBroadcaster(&(this->broadcaster));

	// Threads starting.
	this->fromApplicationLayerDispatcher.startThread();

	this->forwardingSendingSide.startThread();

	this->fromLinkLayerDispatcher.startThread();

	this->forwardingReceivingSide.startThread();

	this->broadcastSendingSide.startThread();

	this->broadcastReceivingSide.startThread();

	this->spanningTreeSendingSide.startThread();

	this->spanningTreeReceivingSide.startThread();

	this->fromPinkAgentDispatcher.startThread();

	this->forwardingUpdateReceiver.startThread();

	this->neighborsInSpanningTreeUpdateReceiver.startThread();

	Log::append("***** Network Layer           ***** < All network layer components were started. >");

	return EXIT_SUCCESS;
}

int NetworkLayer::waitForNetworkComponentsToFinish()
{
	this->fromApplicationLayerDispatcher.waitToFinish();

	this->forwardingSendingSide.waitToFinish();

	this->fromLinkLayerDispatcher.waitToFinish();

	this->forwardingReceivingSide.waitToFinish();

	this->broadcastSendingSide.waitToFinish();

	this->broadcastReceivingSide.waitToFinish();

	this->spanningTreeSendingSide.waitToFinish();

	this->spanningTreeReceivingSide.waitToFinish();

	this->fromPinkAgentDispatcher.waitToFinish();

	this->forwardingUpdateReceiver.waitToFinish();

	this->neighborsInSpanningTreeUpdateReceiver.waitToFinish();

	return EXIT_SUCCESS;
}

Queue<payload_ptr> *NetworkLayer::getAccessPointForApplicationLayer()
{
	return this->fromApplicationLayerDispatcher.getQueue();
}

Queue<payload_ptr> *NetworkLayer::getAccessPointForLinkLayer()
{
	return this->fromLinkLayerDispatcher.getQueue();
}

Queue<payload_ptr> *NetworkLayer::getFromPinkAgentDispatcherQueue()
{
	return this->fromPinkAgentDispatcher.getQueue();
}

void NetworkLayer::setForwardingQueues(Queue<Payload::payload_ptr> *toApplicationAccessPoint, Queue<Payload::payload_ptr> *toLinkLayerAccessPoint)
{
	this->forwardingSendingSide.setQueue(toLinkLayerAccessPoint);
	this->forwardingReceivingSide.setQueues(toApplicationAccessPoint, toLinkLayerAccessPoint);

	this->broadcastSendingSide.setQueue(toLinkLayerAccessPoint);
	this->broadcastReceivingSide.setQueues(toApplicationAccessPoint, toLinkLayerAccessPoint);

	this->spanningTreeSendingSide.setQueue(toLinkLayerAccessPoint);
}

void NetworkLayer::setIdentity(uint16_t greenNodeIdentity)
{
	this->identity = greenNodeIdentity;
	this->forwardingReceivingSide.setIdentity(greenNodeIdentity);
	this->broadcaster.setIdentity(greenNodeIdentity);
}

void NetworkLayer::setPinkConsumerAcessPoint(Queue<Payload::payload_ptr> *pinkConsumerAccessPoint)
{
	this->spanningTreeReceivingSide.setQueue(pinkConsumerAccessPoint);
	this->broadcastReceivingSide.setPinkConsumerAccessPoint(pinkConsumerAccessPoint);
}

void NetworkLayer::setRedirectingPointsForBroadcaster(Queue<Payload::payload_ptr> *toLinkLayerAccessPoint, Queue<Payload::payload_ptr> *toPinkConsumerAcessPoint)
{
	this->broadcaster.setRedirectingQueues(toLinkLayerAccessPoint, toPinkConsumerAcessPoint);
}

void NetworkLayer::setNeighborsIdentity(neighbors_identity_t &neighborsIdentity)
{
	this->neighborsIdentity = neighborsIdentity;
	this->broadcaster.setNeighborsIdentity(neighborsIdentity);
}