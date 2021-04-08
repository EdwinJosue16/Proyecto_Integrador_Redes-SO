#include "SpanningTreeSendingSide.h"
#include "../Log.hpp"
#include "../Payloads/SpanningTreePayload.h"
#include "../Payloads/NetworkPayload.hpp"
#include "../Payloads/PinkPayload.h"

SpanningTreeSendingSide::SpanningTreeSendingSide()
{
	Log::append("***** Spanning Tree Sending Side                ***** < It has been created. >");
}

SpanningTreeSendingSide::~SpanningTreeSendingSide()
{
	Log::append("***** Spanning Tree Sending Side                ***** < It has been destroyed. >");
}

int SpanningTreeSendingSide::run()
{
	Log::append("***** Spanning Tree Sending Side                ***** < It has started. >");
	this->consumeForever();
	Log::append("***** Spanning Tree Sending Side                ***** < It has ended. >");

	return EXIT_SUCCESS;
}

void SpanningTreeSendingSide::consume(const Payload::payload_ptr &otherPayload)
{
	Log::append("***** Spanning Tree Sending Side                ***** < A payload in string format has been received from Pink Producer. >");

	auto pinkPayload = std::static_pointer_cast<PinkPayload>(otherPayload);

	auto spanningTreePayload = std::make_shared<SpanningTreePayload>();

	Log::append("***** Spanning Tree Sending Side                ***** < Message from pink payload " + pinkPayload->pinkMessage + ". >");
	spanningTreePayload->buildFromString(pinkPayload->pinkMessage);
	Log::append("***** Spanning Tree Sending Side                ***** < After building from string.>");
	Log::append("***** Spanning Tree Sending Side                ***** < Spanning tree payload type after building from string: " + std::to_string(spanningTreePayload->type) + ".>");

	auto networkPayload = std::make_shared<NetworkPayload>();

	networkPayload->type = NetworkPayload::SPANNING_TREE_TYPE;
	networkPayload->immediateDestination = spanningTreePayload->destinationId;
	networkPayload->finalDestination = spanningTreePayload->destinationId;
	networkPayload->payloadLength = spanningTreePayload->getBytesRepresentationCount();
	networkPayload->payload = spanningTreePayload;

	this->linkLayerAccessPoint->push(networkPayload);

	Log::append("***** Spanning Tree Sending Side                ***** < A spanning tree payload has been sent to link layer. >");
}

void SpanningTreeSendingSide::setQueue(Queue<Payload::payload_ptr> *linkLayerAccessPoint)
{
	this->linkLayerAccessPoint = linkLayerAccessPoint;
	Log::append("***** Spanning Tree Sending Side                ***** < The redirecting access point to link layer has been received. >");
}
