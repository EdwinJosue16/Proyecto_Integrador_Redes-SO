#ifndef SPANNING_TREE_SENDING_SIDE_H
#define SPANNING_TREE_SENDING_SIDE_H

#include "../../common/Consumer.hpp"
#include "../../common/Payload.hpp"
#include <string>

class SpanningTreeSendingSide : public Consumer<Payload::payload_ptr>
{
private:
	Queue<Payload::payload_ptr> *linkLayerAccessPoint;

public:
	/// Default constructor.
	SpanningTreeSendingSide();

	/// Default destructor.
	~SpanningTreeSendingSide();

public:
	/// It starts thread execution.
	int virtual run() override;

	/// Override this method to process any data extracted from the queue.
	virtual void consume(const Payload::payload_ptr &otherPayload) override;

	/// It initializes the link layers access point pointer.
	void setQueue(Queue<Payload::payload_ptr> *linkLayerAccessPoint);
};

#endif // SPANNING_TREE_SENDING_SIDE_H