#ifndef BROADCAST_RECEIVING_SIDE_H
#define BROADCAST_RECEIVING_SIDE_H

#include "../../common/Consumer.hpp"
#include "../../common/Queue.hpp"
#include "../../common/Payload.hpp"
#include "../CommonTypes.h"
#include "../Payloads/NetworkPayload.hpp"
#include "Broadcaster.h"

class BroadcastReceivingSide : public Consumer<Payload::payload_ptr>
{
private:
	Queue<Payload::payload_ptr> *toApplicationLayerAccessPoint;
	Queue<Payload::payload_ptr> *toLinkLayerAccessPoint;
	Queue<Payload::payload_ptr> *toPinkConsumerAccessPoint;
	Broadcaster *broadcaster;

public:
	/// Default constructor.
	BroadcastReceivingSide();

	/// Default destructor.
	~BroadcastReceivingSide();

	/// Inherited methods.
public:
	/// It starts thread execution.
	int virtual run() override;

	/// Override this method to process any data extracted from the queue.
	virtual void consume(const Payload::payload_ptr &package) override;

	/// It initializes the redirecting queues's pointers.
	void setQueues(Queue<Payload::payload_ptr> *toApplicationLayerAccessPoint, Queue<Payload::payload_ptr> *toLinkLayerAccessPoint);

	void setPinkConsumerAccessPoint(Queue<Payload::payload_ptr> *toPinkConsumerAccessPoint);
	
	void setBroadcaster(Broadcaster *broadcaster);

private:
	/// It sends the broadcast mesasage to application layer.
	void sendPaylaodToApplicationLayer(Payload::payload_ptr payload);

	/// It sends a new payload to each neighbor based on the broadcast original payload.
	void broadcastPayloadToNeighbors(Payload::payload_ptr payload);
};

#endif // BROADCAST_RECEIVING_SIDE_H