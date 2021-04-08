
#include "FromLinkLayerDispatcher.h"
#include "../Payloads/NetworkPayload.hpp"
#include "../Payloads/ForwardingPayload.hpp"
#include "../Payloads/LinkLayerPayload.hpp"
#include "../Log.hpp"

FromLinkLayerDispatcher::FromLinkLayerDispatcher()
{
	Log::append("***** From Link Layer Dispatcher        ***** < It was created. >");
}

FromLinkLayerDispatcher::~FromLinkLayerDispatcher()
{
	Log::append("***** From Link Layer Dispatcher        ***** < It was destroyed. >");
}

Key FromLinkLayerDispatcher::extractKey(const Payload::payload_ptr &data) const
{
	Log::append("***** From Link Layer Dispatcher        ***** < A payload was received from the link layer. >");
	auto networkPayload = std::static_pointer_cast<NetworkPayload>(data);

	if (networkPayload->type == NetworkPayload::FORWARDING_TYPE)
	{
		Log::append("***** From Link Layer Dispatcher        ***** < The payload was redirected to the forwarding component. >");
		return FORWARDING_QUEUE;
	}
	else if (networkPayload->type == NetworkPayload::BROADCAST_TYPE)
	{
		Log::append("***** From Link Layer Dispatcher        ***** < The payload was redirected to the broadcast component. >");
		return BROADCAST_QUEUE;
	}
	else if (networkPayload->type == NetworkPayload::SPANNING_TREE_TYPE)
	{
		Log::append("***** From Link Layer Dispatcher        ***** < The payload was redirected to the spanning tree component. >");
		return SPANNING_TREE_QUEUE;
	}

	return FORWARDING_QUEUE;
}

int FromLinkLayerDispatcher::run()
{
	Log::append("***** From Link Layer Dispatcher        ***** < Its execution will start. >");

	this->consumeForever();

	Log::append("***** From Link Layer Dispatcher        ***** < Its execution has ended. >");
	return EXIT_SUCCESS;
}
