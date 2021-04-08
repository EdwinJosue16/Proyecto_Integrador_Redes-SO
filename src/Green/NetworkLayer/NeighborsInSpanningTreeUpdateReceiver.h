#ifndef NEIGHBORS_IN_SPANNING_TREEUPDATE_RECEIVER_H
#define NEIGHBORS_IN_SPANNING_TREEUPDATE_RECEIVER_H

#include "../../common/Consumer.hpp"
#include "../../common/Payload.hpp"
#include "BroadcastSendingSide.h"
#include "BroadcastReceivingSide.h"
#include "Broadcaster.h"

class NeighborsInSpanningTreeUpdateReceiver : public Consumer<Payload::payload_ptr>
{
private:
	Broadcaster *broadcaster;

public:
	/// Default constructor.
	NeighborsInSpanningTreeUpdateReceiver();

	/// Default constructor.
	~NeighborsInSpanningTreeUpdateReceiver();

	/// Inherited must implement methods.
public:
	/// It starts thread execution.
	int virtual run() override;

	/// Override this method to process any data extracted from the queue
	virtual void consume(const Payload::payload_ptr &package) override;

	void setBroadcaster(Broadcaster* broadcaster);
};

#endif // NEIGHBORS_IN_SPANNING_TREEUPDATE_RECEIVER_H