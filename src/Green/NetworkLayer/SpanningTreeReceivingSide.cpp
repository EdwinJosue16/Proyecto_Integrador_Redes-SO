#include "SpanningTreeReceivingSide.h"
#include "../Payloads/NetworkPayload.hpp"
#include "../Payloads/SpanningTreePayload.h"
#include "../Payloads/PinkPayload.h"
#include "../Log.hpp"

SpanningTreeReceivingSide::SpanningTreeReceivingSide()
{
	Log::append("***** Spanning Tree Receiving Side                ***** < It has been created. >");
}

SpanningTreeReceivingSide::~SpanningTreeReceivingSide()
{
	Log::append("***** Spanning Tree Receiving Side                ***** < It has been destroyed. >");
}

int SpanningTreeReceivingSide::run()
{
	Log::append("***** Spanning Tree Receiving Side                ***** < It has started. >");
	this->consumeForever();
	Log::append("***** Spanning Tree Receiving Side                ***** < It has ended. >");
	return EXIT_SUCCESS;
}

void SpanningTreeReceivingSide::consume(const Payload::payload_ptr &otherPayload)
{
	Log::append("***** Spanning Tree Receiving Side                ***** < A payload was received. >");

	auto networkPayload = std::static_pointer_cast<NetworkPayload>(otherPayload);
	auto spanningTreePayload = std::static_pointer_cast<SpanningTreePayload>(networkPayload->payload);

	auto pinkPayload = std::make_shared<PinkPayload>();
	pinkPayload->pinkMessage = spanningTreePayload->toString();

	this->toPinkAgentAccessPoint->push(pinkPayload);
}

void SpanningTreeReceivingSide::setQueue(Queue<Payload::payload_ptr> *toPinkAgentAccessPoint)
{
	Log::append("***** Spanning Tree Receiving Side                ***** < Pink agent access point was received. >");
	this->toPinkAgentAccessPoint = toPinkAgentAccessPoint;
}