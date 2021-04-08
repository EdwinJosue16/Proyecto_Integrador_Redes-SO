#ifndef NETWORK_LAYER_H
#define NETWORK_LAYER_H

#include "../../common/Queue.hpp"
#include "../../common/Dispatcher.hpp"
#include "FromApplicationLayerDispatcher.h"
#include "FromLinkLayerDispatcher.h"
#include "../Log.hpp"
#include "ForwardingSendingSide.h"
#include "ForwardingReceivingSide.h"
#include "../CommonTypes.h"
#include "BroadcastSendingSide.h"
#include "BroadcastReceivingSide.h"
#include "SpanningTreeSendingSide.h"
#include "FromPinkAgentDispatcher.h"
#include "ForwardingUpdateReceiver.h"
#include "NeighborsInSpanningTreeUpdateReceiver.h"
#include "SpanningTreeReceivingSide.h"
#include "Broadcaster.h"

class NetworkLayer
{
	/// Class members.
private:
	/// It is the access point from messages that come from application layer (blue producer).
	FromApplicationLayerDispatcher fromApplicationLayerDispatcher;
	ForwardingSendingSide forwardingSendingSide;

	/// Broadcast.
	Broadcaster broadcaster;
	BroadcastSendingSide broadcastSendingSide;
	BroadcastReceivingSide broadcastReceivingSide;

	/// Spanning Tree.
	SpanningTreeSendingSide spanningTreeSendingSide;
	SpanningTreeReceivingSide spanningTreeReceivingSide;

	/// Forwarding subcomponents.

	// It is the access point from messages that come from link layer (listener-disptacher).
	FromLinkLayerDispatcher fromLinkLayerDispatcher;
	ForwardingReceivingSide forwardingReceivingSide;

	// Access point to receive message from pink producer.
	FromPinkAgentDispatcher fromPinkAgentDispatcher;

	/// It is used to share forwarding tables updates to forwarding modules.
	ForwardingUpdateReceiver forwardingUpdateReceiver;

	/// It is used to share neighbors in spanning tree table update to broadcast modules.
	NeighborsInSpanningTreeUpdateReceiver neighborsInSpanningTreeUpdateReceiver;

	uint16_t identity;
	neighbors_identity_t neighborsIdentity;

public:
	/// Constructor.
	NetworkLayer();

	/// Destructor.
	~NetworkLayer();

	/// Public interface.
public:
	/// It starts all the inner components of the network layer.
	int start();

	int waitForNetworkComponentsToFinish();

	/// It provides an access point for the inner distpacher that handles incoming messages from application layer.
	Queue<payload_ptr> *getAccessPointForApplicationLayer();

	Queue<payload_ptr> *getAccessPointForLinkLayer();

	/// It provides the FromPinkAgentDispatcher access point
	Queue<payload_ptr> *getFromPinkAgentDispatcherQueue();

	void setForwardingQueues(Queue<Payload::payload_ptr> *toApplicationAccessPoint, Queue<Payload::payload_ptr> *toLinkLayerAccessPoint);

	void setIdentity(uint16_t greenNodeIdentity);

	void setPinkConsumerAcessPoint(Queue<Payload::payload_ptr> *pinkConsumerAccessPoint);

	void setRedirectingPointsForBroadcaster(Queue<Payload::payload_ptr> *toLinkLayerAccessPoint, Queue<Payload::payload_ptr> *toPinkConsumerAcessPoint);

	void setNeighborsIdentity(neighbors_identity_t &neighborsIdentity);
};
#endif // NETWORK_LAYER_H