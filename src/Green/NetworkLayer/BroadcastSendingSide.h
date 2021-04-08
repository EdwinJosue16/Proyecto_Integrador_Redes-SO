#ifndef BROADCAST_SENDING_SIDE_H
#define BROADCAST_SENDING_SIDE_H

#include "../../common/Payload.hpp"
#include "../../common/Consumer.hpp"
#include "../../common/Queue.hpp"
#include "../CommonTypes.h"
#include "Broadcaster.h"

class BroadcastSendingSide : public Consumer<Payload::payload_ptr>
{
private:
	Queue<Payload::payload_ptr> *linkLayerAccessPoint;
	Broadcaster *broadcaster;

public:
	/// Default constructor.
	BroadcastSendingSide();

	/// Default destructor.
	~BroadcastSendingSide();

	/// Inherited methods.
public:
	/// It starts thread execution.
	int virtual run() override;

	/// Override this method to process any data extracted from the queue.
	virtual void consume(const Payload::payload_ptr &package) override;

	/// It initializes the link layers access point pointer.
	void setQueue(Queue<Payload::payload_ptr> *linkLayerAccessPoint);

	void setBroadcaster(Broadcaster *broadcaster);
};

#endif // BROADCAST_SENDING_SIDE_H