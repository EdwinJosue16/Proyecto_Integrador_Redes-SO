#ifndef FORWARDING_PRODUCER_H
#define FORWARDING_PRODUCER_H

#include "../../common/Consumer.hpp"
#include "../../common/Payload.hpp"
#include "../../common/Queue.hpp"
#include "../CommonTypes.h"

class ForwardingReceivingSide : public Consumer<Payload::payload_ptr>
{
private:
	// It is used in case that the forwarding payload reached its destination.
	Queue<Payload::payload_ptr> *toApplicationAccessPoint;
	// It is used in case that the forwarding payload didn't reach its destination.
	Queue<Payload::payload_ptr> *toLinkLayerAccessPoint;

	std::map<DestinationId, ThroughNeighborId> forwardingTable;

	uint16_t identity;

	// ToDo Include forwarding table.

public:
	/// Default constructor.
	ForwardingReceivingSide();

	// Default destructor.
	~ForwardingReceivingSide();

	/// It starts thread execution.
	int virtual run() override;

	/// Override this method to process any data extracted from the queue
	virtual void consume(const Payload::payload_ptr &package) override;

	void setQueues(Queue<Payload::payload_ptr> *toApplicationAccessPoint, Queue<Payload::payload_ptr> *toLinkLayerAccessPoint);

	void setIdentity(uint16_t identity);

	void receiveForwardingTableUpdate(std::map<DestinationId, ThroughNeighborId> &forwardingTable);
};
#endif // FORWARDING_PRODUCER_H