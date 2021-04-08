#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <iostream>

#include "PackagesDispatcher.h"

PackagesDispatcher::PackagesDispatcher()
{
	Log::append("***** Payloads Dispatcher     ***** < was created>");
}

PackagesDispatcher::~PackagesDispatcher()
{
}

int PackagesDispatcher::run()
{
	Log::append("***** Payloads Dispatcher     ***** < has stared>");
	// Dispatch all the network messages we receive to their respective queues
	this->consumeForever();

	// If we exited from the forever loop, the finish message was received
	// For this simulation it is OK to propagate it to all the queues
	//for (const auto &pair : this->toQueues)
	//pair.second->push(base_package_t());

	return EXIT_SUCCESS;
}

int PackagesDispatcher::extractKey(const payload_ptr &data) const
{
	auto linkLayerPayload = std::static_pointer_cast<LinkLayerPayload>(data);

	Log::append("***** Payloads Dispatcher     ***** < A payload was received on the dispatcher. >");

	if (linkLayerPayload->type == LinkLayerPayload::NETWORK_TYPE)
	{
		Log::append("***** Payloads Dispatcher     ***** < A payload was dispached to the network layer. >");
		return NETWORK_LAYER_QUEUE;
	}
	else if (linkLayerPayload->type == LinkLayerPayload::HEARTBEAT_TYPE)
	{
		return HEARTBEAT_COMPONENT_QUEUE;
		Log::append("***** Payloads Dispatcher     ***** < A package was dispached to Hearbeat Component. >");
	}
	else
	{
		Log::append("***** Payloads Dispatcher     ***** < NO DEFINED TYPE. >");
	}

	Log::append("***** Payloads Dispatcher     ***** < A payload was dispached to the network layer. >");

	return NETWORK_LAYER_QUEUE;
}