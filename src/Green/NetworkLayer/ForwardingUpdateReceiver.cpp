#include "ForwardingUpdateReceiver.h"
#include "../Log.hpp"
#include "../Payloads/PinkPayload.h"
#include "../CommonTypes.h"

ForwardingUpdateReceiver::ForwardingUpdateReceiver()
{
	Log::append("***** Forwarding Update Receiver ***** < It was created. >");
}

ForwardingUpdateReceiver::~ForwardingUpdateReceiver()
{
	Log::append("***** Forwarding Update Receiver ***** < It was destroyed. >");
}

int ForwardingUpdateReceiver::run()
{
	Log::append("***** Forwarding Update Receiver ***** < It has started. >");
	this->consumeForever();
	Log::append("***** Forwarding Update Receiver ***** < It has finished. >");

	return EXIT_SUCCESS;
}

void ForwardingUpdateReceiver::consume(const Payload::payload_ptr &package)
{
	auto pinkPayload = std::static_pointer_cast<PinkPayload>(package);
	pinkPayload->removeHeaderOfTableInStringFormat();
	std::map<DestinationId, ThroughNeighborId> forwardingTable =*(
			pinkPayload->makeForwadingTableFromString(pinkPayload->pinkMessage)
	);

	Log::append("***** Forwarding Update Receiver ***** < A forwarding table update has been received. >");

	this->forwardingSendingSide->receiveForwardingTableUpdate(forwardingTable);
	this->forwardingReceivingSide->receiveForwardingTableUpdate(forwardingTable);
}

void ForwardingUpdateReceiver::setForwardingComponents(ForwardingSendingSide *forwardingSendingSide, ForwardingReceivingSide *forwardingReceivingSide)
{
	this->forwardingSendingSide = forwardingSendingSide;
	this->forwardingReceivingSide = forwardingReceivingSide;
}