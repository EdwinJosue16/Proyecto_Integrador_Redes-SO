#ifndef SPANNING_TREE_RECEIVING_SIDE_H
#define SPANNING_TREE_RECEIVING_SIDE_H

#include "../../common/Consumer.hpp"
#include "../../common/Payload.hpp"
#include "../../common/Queue.hpp"
#include "BroadcastReceivingSide.h"

class SpanningTreeReceivingSide : public Consumer<Payload::payload_ptr>
{

private:
	/// It is the access point to pink consumer agent.
	Queue<Payload::payload_ptr> *toPinkAgentAccessPoint;

public:
	/// Default constructor.
	SpanningTreeReceivingSide();

	/// Default destructor.
	~SpanningTreeReceivingSide();

public:
	/// It starts thread execution.
	int virtual run() override;

	/// Override this method to process any data extracted from the queue.
	virtual void consume(const Payload::payload_ptr &otherPayload) override;

	/// It initializes the link layers access point pointer.
	void setQueue(Queue<Payload::payload_ptr> *toPinkAgentAccessPoint);
};

#endif // SPANNING_TREE_RECEIVING_SIDE_H