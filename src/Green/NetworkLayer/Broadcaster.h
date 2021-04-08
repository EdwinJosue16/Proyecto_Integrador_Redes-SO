#ifndef BROADCASTER_H
#define BROADCASTER_H

#include "../CommonTypes.h"
#include "../../common/Queue.hpp"
#include "../../common/Payload.hpp"
#include "../Payloads/NetworkPayload.hpp"
#include "../Payloads/BroadcastPayload.h"
class Broadcaster
{
public:
	/// Constants
	static const uint8_t BROADCAST_TO_ALL_NEIGHBORS = 0;
	static const uint8_t BROADCAST_TO_NEIGHBORS_IN_SPANNING_TREE = 1;

private:
	/// Accesses points to redirect broadcast type network payloads.
	Queue<Payload::payload_ptr> *toLinkLayerAccessPoint;
	Queue<Payload::payload_ptr> *toPinkConsumerAcessPoint;

	neighbors_identity_t neighborsIdentity;

	std::vector<uint16_t> neighborsInSpanningTree;

	uint16_t greenNodeIdentity;

public:
	/// Default constructor.
	Broadcaster();

	/// Default destructor.
	~Broadcaster();

	void setRedirectingQueues(Queue<Payload::payload_ptr> *toLinkLayerAccessPoint, Queue<Payload::payload_ptr> *toPinkConsumerAcessPoint);

	void setNeighborsIdentity(neighbors_identity_t &neighborIdentity);

	void setIdentity(uint16_t greenNodeIdentity);

	void setNeighborsInSpanningTree(std::vector<uint16_t> &neighborsInSpanningTree);

	void broadcast(Payload::payload_ptr payloadToBroadcast, uint8_t toWhichNeighbors);

	void broadcastReachabilityToNeighborsInSpanningTree();

private:
	std::shared_ptr<NetworkPayload> buildNetworkPayloadForBroadcast(uint16_t destinationId, std::shared_ptr<BroadcastPayload> broadcastPayload);

	/// Broadcast sending side.
	void broadCastFromSendingSide(Payload::payload_ptr payloadToBroadcast, uint8_t toWhichNeighbors);
	void broadcastToAllNeighborsSendingSide(Payload::payload_ptr payloadToBroadcast);
	void broadcastToNeighborsInSpanningTreeSendingSide(Payload::payload_ptr payloadToBroadcast);

	/// Broadcast receiving side.
	void broadCastFromReceivingSide(Payload::payload_ptr payloadToBroadcast, uint8_t toWhichNeighbors);
	void broadcastToAllNeighborsReceivingSide(Payload::payload_ptr payloadToBroadcast);
	void broadcastToNeighborsInSpanningTreeReceivingSide(Payload::payload_ptr payloadToBroadcast);
};
#endif // BROADCASTER_H