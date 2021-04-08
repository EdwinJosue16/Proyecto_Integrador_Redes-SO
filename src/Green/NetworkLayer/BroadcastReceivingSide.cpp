#include "BroadcastReceivingSide.h"
#include "../Log.hpp"
#include "../Payloads/BroadcastPayload.h"
#include "../Payloads/BroadcastMessagePayload.h"
#include "../Payloads/ApplicationPayload.h"
#include "../Payloads/BroadcastReachabilityPayload.h"
#include "../Payloads/PinkPayload.h"

BroadcastReceivingSide::BroadcastReceivingSide()
{
	Log::append("***** Broadcast Receiving Side              ***** < It has been created. >");
}

BroadcastReceivingSide::~BroadcastReceivingSide()
{
	Log::append("***** Broadcast Receiving Side              ***** < It has been destroyed. >");
}

int BroadcastReceivingSide::run()
{
	Log::append("***** Broadcast Receiving Side              ***** < It has started. >");
	this->consumeForever();
	Log::append("***** Broadcast Receiving Side              ***** < It has ended. >");
	return EXIT_SUCCESS;
}

void BroadcastReceivingSide::sendPaylaodToApplicationLayer(Payload::payload_ptr payload)
{
	auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(payload);
	auto broadcastMessagePayload = std::static_pointer_cast<BroadcastMessagePayload>(broadcastPayload->payload);

	// It sends the message to application layer.
	auto applicationPayload = std::make_shared<ApplicationPayload>();
	applicationPayload->message = broadcastMessagePayload->message;
	applicationPayload->sourceId = broadcastPayload->sourceId;
	applicationPayload->type = BROADCAST_TYPE;

	this->toApplicationLayerAccessPoint->push(applicationPayload);
	Log::append("***** Broadcast Receiving Side              ***** < A payload was sent to blue consumer. >");
}

void BroadcastReceivingSide::broadcastPayloadToNeighbors(Payload::payload_ptr payload)
{
	auto networkPayload = std::static_pointer_cast<NetworkPayload>(payload);
	auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(networkPayload->payload);
	this->broadcaster->broadcast(broadcastPayload, Broadcaster::BROADCAST_TO_NEIGHBORS_IN_SPANNING_TREE);
}

void BroadcastReceivingSide::consume(const Payload::payload_ptr &package)
{
	Log::append("***** Broadcast Receiving Side              ***** < A payload was received. >");

	auto networkPayload = std::static_pointer_cast<NetworkPayload>(package);
	auto broadcastPayload = std::static_pointer_cast<BroadcastPayload>(networkPayload->payload);

	if (broadcastPayload->type == BroadcastPayload::BROADCAST_MESSAGE_TYPE)
	{
		this->sendPaylaodToApplicationLayer(broadcastPayload);
		this->broadcastPayloadToNeighbors(networkPayload);
	}
	else if (broadcastPayload->type == BroadcastPayload::REACHABILITY_TYPE)
	{
		Log::append("***** Broadcast Receiving Side              ***** < A reachability from neighbor " + std::to_string(broadcastPayload->sourceId) + " was received. >");
		auto broadcastReachabilityPayload = std::static_pointer_cast<BroadcastReachabilityPayload>(broadcastPayload->payload);

		// It sends the neighbors reachability update to pink node.
		auto pinkPayload = std::make_shared<PinkPayload>();
		pinkPayload->pinkMessage = broadcastReachabilityPayload->toString();
		this->toPinkConsumerAccessPoint->push(pinkPayload);

		// ToDo To implement sending new neighbors from reachability to blue node.

		this->broadcaster->broadcast(broadcastPayload, Broadcaster::BROADCAST_TO_NEIGHBORS_IN_SPANNING_TREE);
	}
}

void BroadcastReceivingSide::setQueues(Queue<Payload::payload_ptr> *toApplicationLayerAccessPoint, Queue<Payload::payload_ptr> *toLinkLayerAccessPoint)
{
	this->toApplicationLayerAccessPoint = toApplicationLayerAccessPoint;
	this->toLinkLayerAccessPoint = toLinkLayerAccessPoint;

	Log::append("***** Broadcast Receiving Side              ***** < Redirecting queues have been received. >");
}

void BroadcastReceivingSide::setPinkConsumerAccessPoint(Queue<Payload::payload_ptr> *toPinkConsumerAccessPoint)
{
	this->toPinkConsumerAccessPoint = toPinkConsumerAccessPoint;
}

void BroadcastReceivingSide::setBroadcaster(Broadcaster *broadcaster)
{
	this->broadcaster = broadcaster;
}