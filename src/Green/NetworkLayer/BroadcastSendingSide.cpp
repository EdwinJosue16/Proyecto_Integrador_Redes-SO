#include "BroadcastSendingSide.h"
#include "../Log.hpp"
#include "../Payloads/ApplicationPayload.h"
#include "../Payloads/BroadcastMessagePayload.h"
#include "../Payloads/BroadcastPayload.h"
#include "../Payloads/NetworkPayload.hpp"

BroadcastSendingSide::BroadcastSendingSide()
{
	Log::append("***** Broadcast Sending Side                ***** < It has been created. >");
}

BroadcastSendingSide::~BroadcastSendingSide()
{
	Log::append("***** Broadcast Sending Side                ***** < It has been destroyed. >");
}

int BroadcastSendingSide::run()
{
	Log::append("***** Broadcast Sending Side                ***** < It has started. >");
	this->consumeForever();
	Log::append("***** Broadcast Sending Side                ***** < It has ended. >");
	return EXIT_SUCCESS;
}

void BroadcastSendingSide::consume(const Payload::payload_ptr &package)
{
	Log::append("***** Broadcast Sending Side                ***** < An application payload was received. >");

	auto applicationPayload = std::static_pointer_cast<ApplicationPayload>(package);

	// It builds the broadcast message payload.
	auto broadcastMessagePayload = std::make_shared<BroadcastMessagePayload>();
	broadcastMessagePayload->message = applicationPayload->message;
	
	// It builds the broadcast payload.
	auto broadcastPayload = std::make_shared<BroadcastPayload>();
	broadcastPayload->type = BroadcastPayload::BROADCAST_MESSAGE_TYPE;
	broadcastPayload->broadcastSide = BroadcastPayload::BROADCAST_SENDING_SIDE;
	broadcastPayload->sourceId = applicationPayload->sourceId;
	broadcastPayload->payloadLength = broadcastMessagePayload->getBytesRepresentationCount();
	broadcastPayload->payload = broadcastMessagePayload;

	this->broadcaster->broadcast(broadcastPayload, Broadcaster::BROADCAST_TO_NEIGHBORS_IN_SPANNING_TREE);
}

void BroadcastSendingSide::setQueue(Queue<Payload::payload_ptr> *linkLayerAccessPoint)
{
	Log::append("***** Broadcast Sending Side                ***** < Redirecting queue has been received. >");
	this->linkLayerAccessPoint = linkLayerAccessPoint;
}

void BroadcastSendingSide::setBroadcaster(Broadcaster *broadcaster)
{
	this->broadcaster = broadcaster;
}