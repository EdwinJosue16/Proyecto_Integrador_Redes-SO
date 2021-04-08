#ifndef APPLICATION_FORWARDING_CONSUMER_H
#define APPLICATION_FORWARDING_CONSUMER_H

#include "../../common/Consumer.hpp"
#include "../../common/Payload.hpp"
#include "../../common/Queue.hpp"
#include "../CommonTypes.h"
class ForwardingSendingSide : public Consumer<Payload::payload_ptr>
{

private:
	// It is used to redirect the created payload to the link layer.
	Queue<Payload::payload_ptr> *toLinkLayerAccessPoint;

	std::map<DestinationId, ThroughNeighborId> forwardingTable;

public:
	/// Default constructor.
	ForwardingSendingSide();

	/// Default destructor.
	~ForwardingSendingSide();

public:
	static const uint8_t FORWARDING_TYPE = 1;
	static const uint8_t NETWORK_TYPE = 2;

public:
	/// It starts thread execution.
	int virtual run() override;

	/// Override this method to process any data extracted from the queue
	virtual void consume(const Payload::payload_ptr &package) override;

	/// Set queue where network payloads will be produced.
	void setQueue(Queue<Payload::payload_ptr> *toLinkLayerAccessPoint);

	/// It receives a forwarding table update.
	void receiveForwardingTableUpdate(std::map<DestinationId, ThroughNeighborId> &forwardingTable);
};
#endif // APPLICATION_FORWARDING_CONSUMER_H