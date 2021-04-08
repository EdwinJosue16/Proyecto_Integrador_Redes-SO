#ifndef PINK_AGENT
#define PINK_AGENT

#include <vector>
#include <string>
#include <utility>
#include "../common/Queue.hpp"
#include "../common/Thread.hpp"
#include "PinkProducer.hpp"
#include "PinkConsumer.h"
class PinkAgent : public Thread
{
private:
	PinkProducer *pinkProducer;
	PinkConsumer *pinkConsumer;
	uint16_t greenNodeId;

public:
	///
	PinkAgent(PinkProducer *pinkProducer, PinkConsumer *pinkConsumer, int16_t &greenNodeId);

	///
	~PinkAgent();

	///
	virtual int run() override;

	///
	void work();
};

#endif