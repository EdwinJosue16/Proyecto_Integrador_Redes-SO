#include "ForwardingReceivingSide.h"
#include "../Log.hpp"
#include "../Payloads/ForwardingPayload.hpp"
#include "../Payloads/LinkLayerPayload.hpp"
#include "../Payloads/NetworkPayload.hpp"
#include "../Payloads/ApplicationPayload.h"
#include "../Payloads/ApplicationForwardingPayload.h"
#include "../Payloads/ApplicationShortMessagePayload.h"

ForwardingReceivingSide::ForwardingReceivingSide()
	: toApplicationAccessPoint{nullptr},
	  toLinkLayerAccessPoint{nullptr}
{
	Log::append("***** Link Layer Forwarding Consumer        ***** < It was created. >");
}

ForwardingReceivingSide::~ForwardingReceivingSide()
{
	Log::append("***** Link Layer Forwarding Consumer        ***** < It was destroyed. >");
}

int ForwardingReceivingSide::run()
{
	Log::append("***** Link Layer Forwarding Consumer        ***** < It has started. >");
	this->consumeForever();
	Log::append("***** Link Layer Forwarding Consumer        ***** < It has ended. >");

	return EXIT_SUCCESS;
}

void ForwardingReceivingSide::consume(const Payload::payload_ptr &package)
{
	Log::append("***** Link Layer Forwarding Consumer        ***** < A payload was received. >");

	auto networkPayload = std::static_pointer_cast<NetworkPayload>(package);
	auto forwardingPayload = std::static_pointer_cast<ForwardingPayload>(networkPayload->payload);
	auto applicationForwardingPayload = std::static_pointer_cast<ApplicationForwardingPayload>(forwardingPayload->payload);
	auto applicationShortMessagePayload = std::static_pointer_cast<ApplicationShortMessagePayload>(applicationForwardingPayload->payload);

	if (forwardingPayload->destinationId == this->identity) // The destination was reached.
	{
		auto applicationPayload = std::make_shared<ApplicationPayload>();
		applicationPayload->message = std::string(applicationShortMessagePayload->message);
		applicationPayload->sourceId = forwardingPayload->sourceId;
		
		// The network payload is sent to the upper layer.
		Log::append("***** Link Layer Forwarding Consumer        ***** < A payload was sent to the application layer (BLUE CONSUMER). >");
		this->toApplicationAccessPoint->push(applicationPayload);
	}
	else
	{
		// Check forwarding table and determine the immediate destination.
		uint16_t immediateNeighbor = this->forwardingTable.find(forwardingPayload->destinationId)->second;//
		networkPayload->immediateDestination = immediateNeighbor; //
		Log::append("***** Link Layer Forwarding Consumer        ***** < The payload was forwarded to the link layer. >");
		this->toLinkLayerAccessPoint->push(networkPayload);//
	}
}

void ForwardingReceivingSide::setQueues(Queue<Payload::payload_ptr> *toApplicationAccessPoint, Queue<Payload::payload_ptr> *toLinkLayerAccessPoint)
{
	this->toApplicationAccessPoint = toApplicationAccessPoint;
	this->toLinkLayerAccessPoint = toLinkLayerAccessPoint;
	Log::append("***** Link Layer Forwarding Consumer        ***** < Queues have been received. >");
}

void ForwardingReceivingSide::setIdentity(uint16_t identity)
{
	this->identity = identity;
}

void ForwardingReceivingSide::receiveForwardingTableUpdate(std::map<DestinationId, ThroughNeighborId> &forwardingTable)
{
	Log::append("***** Forwarding Receiving Side     ***** < A forwarding table update has been received. >");
	this->forwardingTable = forwardingTable;
}
