#include "fromOtherNodesMiniDispatcher.h"
#include "Payloads/LinkLayerPayload.hpp"
#include "Payloads/NetworkPayload.hpp"
#include "Log.hpp"

FromOtherNodesMiniDispatcher::FromOtherNodesMiniDispatcher()
{
	Log::append("***** From Other Nodes Mini Dispatcher      ***** < It was created. >");
}

FromOtherNodesMiniDispatcher::~FromOtherNodesMiniDispatcher()
{
	Log::append("***** From Other Nodes Mini Dispatcher      ***** < It was destroyed. >");
}

void FromOtherNodesMiniDispatcher::setRedirectingQueues(Queue<Payload::payload_ptr> *toHearbeatComponentAccessPoint,
														Queue<Payload::payload_ptr> *toNetworkLayerAccessPoint)
{
	this->toHearbeatComponentAccessPoint = toHearbeatComponentAccessPoint;
	this->toNetworkLayerAccessPoint = toNetworkLayerAccessPoint;
	Log::append("***** From Other Nodes Mini Dispatcher      ***** < The redirecting queues were initialized. >");
}

void FromOtherNodesMiniDispatcher::consume(const Payload::payload_ptr &payload)
{
	auto linkPayload = std::static_pointer_cast<LinkLayerPayload>(payload);

	if (linkPayload->type == LinkLayerPayload::HEARTBEAT_TYPE)
	{
		this->toHearbeatComponentAccessPoint->push(linkPayload);
		Log::append("***** From Other Nodes Mini Dispatcher      ***** < A link layer payload was redirected to hearbeat component. >");
	}
	else if (linkPayload->type == LinkLayerPayload::NETWORK_TYPE)
	{
		// It removes the link layer header.
		auto networkPayload = std::static_pointer_cast<NetworkPayload>(linkPayload->payload);
		this->toNetworkLayerAccessPoint->push(networkPayload);
		Log::append("***** From Other Nodes Mini Dispatcher      ***** < A link network payload was redirected to network layer. >");
	}
}

int FromOtherNodesMiniDispatcher::run()
{
	Log::append("***** From Other Nodes Mini Dispatcher      ***** < It will start running. >");
	this->consumeForever();
	Log::append("***** From Other Nodes Mini Dispatcher      ***** < It has finished. >");
	return EXIT_SUCCESS;
}
