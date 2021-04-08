#include "NeighborsInSpanningTreeUpdateReceiver.h"
#include "../Log.hpp"
#include "../Payloads/PinkPayload.h"

NeighborsInSpanningTreeUpdateReceiver::NeighborsInSpanningTreeUpdateReceiver()
{
	Log::append("***** Broadcast Update Receiver ***** < It was started. >");
}

NeighborsInSpanningTreeUpdateReceiver::~NeighborsInSpanningTreeUpdateReceiver()
{
	Log::append("***** Broadcast Update Receiver ***** < It was destroyed. >");
}

int NeighborsInSpanningTreeUpdateReceiver::run()
{
	Log::append("***** Broadcast Update Receiver ***** < It has started. >");
	this->consumeForever();
	Log::append("***** Broadcast Update Receiver ***** < It has finished. >");

	return EXIT_SUCCESS;
}

void NeighborsInSpanningTreeUpdateReceiver::consume(const Payload::payload_ptr &package)
{
	Log::append("***** Broadcast Update Receiver ***** < A pink payload has been received. >");

	auto pinkPayload = std::static_pointer_cast<PinkPayload>(package);

	pinkPayload->removeHeaderOfTableInStringFormat();

	std::vector<uint16_t> neighborsInSpanningTreeUpdate = *(
		pinkPayload->neighborsInSpanningTreeUpdateFromString(pinkPayload->pinkMessage));

	Log::append("***** Broadcast Update Receiver ***** < A forwarding table update has been sent to forwarding components. >");

	this->broadcaster->setNeighborsInSpanningTree(neighborsInSpanningTreeUpdate);
	this->broadcaster->broadcastReachabilityToNeighborsInSpanningTree();
}

void NeighborsInSpanningTreeUpdateReceiver::setBroadcaster(Broadcaster *broadcaster)
{
	this->broadcaster = broadcaster;
}