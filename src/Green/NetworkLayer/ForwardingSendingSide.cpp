#include "ForwardingSendingSide.h"
#include "../Log.hpp"
#include "../Payloads/ApplicationPayload.h"
#include "../Payloads/ForwardingPayload.hpp"
#include "../Payloads/NetworkPayload.hpp"
#include "../Payloads/LinkLayerPayload.hpp"
#include "../Payloads/ApplicationShortMessagePayload.h"
#include "../Payloads/ApplicationForwardingPayload.h"

#include <cstring> //strcpy

ForwardingSendingSide::ForwardingSendingSide()
	: toLinkLayerAccessPoint{nullptr}
{
	Log::append("***** Application Layer Forwarding Consumer ***** < It was created. >");
}

ForwardingSendingSide::~ForwardingSendingSide()
{
	Log::append("***** Application Layer Forwarding Consumer ***** < It was destroyed. >");
}

int ForwardingSendingSide::run()
{
	Log::append("***** Application Layer Forwarding Consumer ***** < It has started. >");
	this->consumeForever();
	Log::append("***** Application Layer Forwarding Consumer ***** < It has ended. >");

	return EXIT_SUCCESS;
}

void ForwardingSendingSide::consume(const Payload::payload_ptr &package)
{
	Log::append("***** Application Layer Forwarding Consumer ***** < An application payload was received. >");

	auto appPayload = std::static_pointer_cast<ApplicationPayload>(package);

	// Short message payload
	auto applicationShortMessagePayload = std::make_shared<ApplicationShortMessagePayload>();

	applicationShortMessagePayload->message = appPayload->message;

	// Application forwarding payload
	auto applicationForwardingPayload = std::make_shared<ApplicationForwardingPayload>();
	
	applicationForwardingPayload->type = ApplicationForwardingPayload::SHORT_MESSAGE_TRANSMISSION;
	applicationForwardingPayload->payloadLength = applicationShortMessagePayload->getBytesRepresentationCount();
	applicationForwardingPayload->payload = applicationShortMessagePayload;

	// Forwarding payload building.
	auto forwardingPayload = std::make_shared<ForwardingPayload>();

	forwardingPayload->sourceId = (uint16_t)(appPayload->sourceId);
	forwardingPayload->destinationId = appPayload->destinationId;
	forwardingPayload->payloadLength = applicationForwardingPayload->getBytesRepresentationCount();
	forwardingPayload->payload = applicationForwardingPayload;
	// -------------------------------------------------------- o --------------------------------------------------------

	// Network layer header building.
	auto networkLayerPayload = std::make_shared<NetworkPayload>();

	networkLayerPayload->finalDestination = forwardingPayload->destinationId;
	networkLayerPayload->type = NetworkPayload::FORWARDING_TYPE;
	networkLayerPayload->payloadLength = forwardingPayload->getBytesRepresentationCount();
	networkLayerPayload->payload = forwardingPayload;
	
	Log::append("***** Application Layer Forwarding Consumer ***** < About to check forw. table>");
	auto immediateNeighbor = this->forwardingTable.find(forwardingPayload->destinationId)->second; //
	networkLayerPayload->immediateDestination = immediateNeighbor; //
	Log::append("***** Application Layer Forwarding Consumer ***** < Forwarding package to>"+ std::to_string(immediateNeighbor));
	
	// In the meantime, in the future this value will be provided by the forwarding table.
	//networkLayerPayload->immediateDestination = forwardingPayload->destinationId;

	Log::append("***** Application Layer Forwarding Consumer ***** < A link network payload was built and redirected to the link layer access point. >");

	this->toLinkLayerAccessPoint->push(networkLayerPayload);
}

void ForwardingSendingSide::setQueue(Queue<Payload::payload_ptr> *toLinkLayerAccessPoint)
{
	this->toLinkLayerAccessPoint = toLinkLayerAccessPoint;
	Log::append("***** Application Layer Forwarding Consumer ***** < Queue has been received. >");
}

void ForwardingSendingSide::receiveForwardingTableUpdate(std::map<DestinationId, ThroughNeighborId> &forwardingTable)
{
	this->forwardingTable = forwardingTable;
}