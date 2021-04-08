#include "FromApplicationLayerDispatcher.h"

FromApplicationLayerDispatcher::FromApplicationLayerDispatcher()
{
	Log::append("***** From Application Layer Dispatcher ***** < It was constructed. >");
}

FromApplicationLayerDispatcher::~FromApplicationLayerDispatcher()
{
	Log::append("***** From Application Layer Dispatcher ***** < It was destroyed. >");
}

int FromApplicationLayerDispatcher::run()
{
	Log::append("***** From Application Layer Dispatcher ***** < It has started. >");
	this->consumeForever();
	return EXIT_SUCCESS;
}

int FromApplicationLayerDispatcher::extractKey(const std::shared_ptr<Payload> &data) const
{
	if (data->type == BROADCAST_TYPE)
	{
		Log::append("***** From Application Layer Dispatcher ***** < The payload was redirected to the Broadcast Component. >");
		return BROADCAST_QUEUE;
	}

	Log::append("***** From Application Layer Dispatcher ***** < The payload was redirected to the Forwarding Component. >");

	return FORWARDING_QUEUE;
}
