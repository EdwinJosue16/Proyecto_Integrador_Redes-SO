#ifndef BROADCAST_H
#define BROADCAST_H

#include "../../common/Consumer.hpp"
#include "../../common/Queue.hpp"
#include "../CommonTypes.h"

#include <vector>

class Broadcast : public Consumer<payload_ptr>
{
	/// Class members.
private:
	/// Neighbors ids that form part of the local spanning tree.
	std::vector<int16_t> spanningTree;
	/// Link layer access point queue.
	Queue<payload_ptr> *linkLayerAccessPoint;

public:
	/// It initializes the pointer to the link layer access point.
	void setLinkLayerAcessPoint(Queue<payload_ptr> *linkLayerAccessPoint);

public:
	/// Constructor.
	Broadcast();
	/// Destructor.
	~Broadcast();

private:
	/// It replicates the message for every neighbor that forms part of the local spanning tree.
	virtual void consume(const payload_ptr &package);

	/// It consumes broadcast packages (if available) indefinitely.
	virtual int run();
};
#endif // BROADCAST_H