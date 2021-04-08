#include "FromPinkAgentDispatcher.h"
#include "../Log.hpp"
#include "../Payloads/PinkPayload.h"

FromPinkAgentDispatcher::FromPinkAgentDispatcher()
{
	Log::append("***** From Pink Agent Dispatcher ***** < It was constructed. >");
}

FromPinkAgentDispatcher::~FromPinkAgentDispatcher()
{
	Log::append("***** From Pink Agent Dispatcher ***** < It was destroyed. >");
}

int FromPinkAgentDispatcher::run()
{
	Log::append("***** From Pink Agent Dispatcher ***** < It has started. >");
	this->consumeForever();
	Log::append("***** From Pink Agent Dispatcher ***** < It has finished. >");

	return EXIT_SUCCESS;
}

int FromPinkAgentDispatcher::extractKey(const std::shared_ptr<Payload> &data) const
{
	Log::append("***** From Pink Agent Dispatcher ***** < A Pink Payload was received. >");

	auto pinkPayload = std::static_pointer_cast<PinkPayload>(data);

	if (pinkPayload->type == PinkPayload::SPANNING_TREE_MESSAGE)
	{
		Log::append("***** From Pink Agent Dispatcher ***** < It was redirected to spanning tree module. >");
		return FromPinkAgentDispatcher::SPANNING_TREE_QUEUE;
	}
	else if (pinkPayload->type == PinkPayload::NEIGHBORS_IN_SPANNING_TREE_TABLE_UPDATE)
	{
		Log::append("***** From Pink Agent Dispatcher ***** < It was redirected to broadcast module. >");
		return FromPinkAgentDispatcher::BROADCAST_QUEUE;
	}
	else if (pinkPayload->type == PinkPayload::FORWARDING_TABLE_UPDATE)
	{
		Log::append("***** From Pink Agent Dispatcher ***** < It was redirected to forwarding module. >");
		return FromPinkAgentDispatcher::FORWARDING_QUEUE;
	}

	return EXIT_SUCCESS;
}
