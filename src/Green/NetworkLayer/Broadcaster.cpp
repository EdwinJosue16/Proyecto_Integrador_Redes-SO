#include "Broadcaster.h"
#include "../Payloads/BroadcastReachabilityPayload.h"
#include "../Payloads/PinkPayload.h"
#include "../Log.hpp"

Broadcaster::Broadcaster()
{
	Log::append("***** Broadcaster ***** < It has been created. >");
}

Broadcaster::~Broadcaster()
{
	Log::append("***** Broadcaster ***** < It has been destroyed. >");
}

void Broadcaster::setRedirectingQueues(Queue<Payload::payload_ptr> *toLinkLayerAccessPoint, Queue<Payload::payload_ptr> *toPinkConsumerAcessPoint)
{
	this->toLinkLayerAccessPoint = toLinkLayerAccessPoint;
	this->toPinkConsumerAcessPoint = toPinkConsumerAcessPoint;
}

void Broadcaster::setNeighborsIdentity(neighbors_identity_t &neighborIdentity)
{
	this->neighborsIdentity = neighborIdentity;
}

void Broadcaster::setIdentity(uint16_t greenNodeIdentity)
{
	this->greenNodeIdentity = greenNodeIdentity;
}

void Broadcaster::setNeighborsInSpanningTree(std::vector<uint16_t> &neighborsInSpanningTree)
{
	this->neighborsInSpanningTree = neighborsInSpanningTree;
}

void Broadcaster::broadcast(Payload::payload_ptr payloadToBroadcast, uint8_t toWhichNeighbors)
{
	auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(payloadToBroadcast);

	if (broadcastPayload->broadcastSide == BroadcastPayload::BROADCAST_SENDING_SIDE)
	{
		this->broadCastFromSendingSide(payloadToBroadcast, toWhichNeighbors);
	}
	else if (broadcastPayload->broadcastSide == BroadcastPayload::BROADCAST_RECEIVING_SIDE)
	{
		this->broadCastFromReceivingSide(payloadToBroadcast, toWhichNeighbors);
	}
}

void Broadcaster::broadCastFromSendingSide(Payload::payload_ptr payloadToBroadcast, uint8_t toWhichNeighbors)
{
	if (toWhichNeighbors == Broadcaster::BROADCAST_TO_ALL_NEIGHBORS)
	{
		broadcastToAllNeighborsSendingSide(payloadToBroadcast);
	}
	else if (toWhichNeighbors == Broadcaster::BROADCAST_TO_NEIGHBORS_IN_SPANNING_TREE)
	{
		broadcastToNeighborsInSpanningTreeSendingSide(payloadToBroadcast);
	}
}

void Broadcaster::broadcastToAllNeighborsSendingSide(Payload::payload_ptr payloadToBroadcast)
{
	auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(payloadToBroadcast);

	for (auto &neighbor : this->neighborsIdentity)
	{
		this->toLinkLayerAccessPoint->push(this->buildNetworkPayloadForBroadcast(neighbor.id, broadcastPayload));
	}
}

void Broadcaster::broadcastToNeighborsInSpanningTreeSendingSide(Payload::payload_ptr payloadToBroadcast)
{
	auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(payloadToBroadcast);

	for (auto &neighborId : this->neighborsInSpanningTree)
	{
		Log::append("*BROADCASTER: Sending side* Sending broadcast message to ST neighbor  #"+ std::to_string(neighborId));
		this->toLinkLayerAccessPoint->push(this->buildNetworkPayloadForBroadcast(neighborId, broadcastPayload));
	}
}

void Broadcaster::broadCastFromReceivingSide(Payload::payload_ptr payloadToBroadcast, uint8_t toWhichNeighbors)
{
	if (toWhichNeighbors == Broadcaster::BROADCAST_TO_ALL_NEIGHBORS)
	{
		broadcastToAllNeighborsReceivingSide(payloadToBroadcast);
	}
	else if (toWhichNeighbors == Broadcaster::BROADCAST_TO_NEIGHBORS_IN_SPANNING_TREE)
	{
		broadcastToNeighborsInSpanningTreeReceivingSide(payloadToBroadcast);
	}
}

void Broadcaster::broadcastToAllNeighborsReceivingSide(Payload::payload_ptr payloadToBroadcast)
{
	auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(payloadToBroadcast);

	for (auto &neighborIdentity : this->neighborsIdentity)
	{
		if (neighborIdentity.id != broadcastPayload->immediateSource)
		{
			this->toLinkLayerAccessPoint->push(this->buildNetworkPayloadForBroadcast(neighborIdentity.id, broadcastPayload));
		}
	}
}

void Broadcaster::broadcastToNeighborsInSpanningTreeReceivingSide(Payload::payload_ptr payloadToBroadcast)
{
	auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(payloadToBroadcast);

	for (auto &neighborId : this->neighborsInSpanningTree)
	{
		if (neighborId != broadcastPayload->immediateSource)
		{
			Log::append("*BROADCASTER: Receiving side* Sending broadcast message to ST neighbor  #"+ std::to_string(neighborId));
			this->toLinkLayerAccessPoint->push(this->buildNetworkPayloadForBroadcast(neighborId, broadcastPayload));
		}
	}
}

std::shared_ptr<NetworkPayload> Broadcaster::buildNetworkPayloadForBroadcast(uint16_t destinationId, std::shared_ptr<BroadcastPayload> broadcastPayload)
{
	auto networkPayload = std::make_shared<NetworkPayload>();
	networkPayload->type = NetworkPayload::BROADCAST_TYPE;
	networkPayload->payloadLength = broadcastPayload->getBytesRepresentationCount();
	networkPayload->payload = broadcastPayload;

	networkPayload->finalDestination = destinationId;
	networkPayload->immediateDestination = destinationId;

	return networkPayload;
}

void Broadcaster::broadcastReachabilityToNeighborsInSpanningTree()
{
	// It builds the reachability payload.
	auto broadcastReachabilityPayload = std::make_shared<BroadcastReachabilityPayload>();

	broadcastReachabilityPayload->initializeReachability(this->neighborsIdentity);
	broadcastReachabilityPayload->numberOfNeighbors = (uint16_t)this->neighborsIdentity.size();
	broadcastReachabilityPayload->broadcastSourceId = (uint16_t)this->greenNodeIdentity;

	// It builds the broadcast payload.
	auto broadcastPayload = std::make_shared<BroadcastPayload>();

	broadcastPayload->type = BroadcastPayload::REACHABILITY_TYPE;
	broadcastPayload->sourceId = this->greenNodeIdentity;
	broadcastPayload->payloadLength = broadcastReachabilityPayload->getBytesRepresentationCount();
	broadcastPayload->payload = broadcastReachabilityPayload;

	// It shares reachability to *this pink node.
	auto pinkPayload = std::make_shared<PinkPayload>();
	pinkPayload->pinkMessage = broadcastReachabilityPayload->toString();
	this->toPinkConsumerAcessPoint->push(pinkPayload);

	// It broadcast *this reachability to all neighbors in spanning tree.
	for (auto &neighborId : this->neighborsInSpanningTree)
	{
		this->toLinkLayerAccessPoint->push(this->buildNetworkPayloadForBroadcast(neighborId, broadcastPayload));
	}
}
