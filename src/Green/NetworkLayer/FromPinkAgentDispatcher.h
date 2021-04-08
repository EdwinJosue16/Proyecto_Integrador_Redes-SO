#ifndef FROM_PINK_AGENT_DISPTACHER_H
#define FROM_PINK_AGENT_DISPTACHER_H

#include "../../common/Dispatcher.hpp"
#include "../../common/Payload.hpp"

class FromPinkAgentDispatcher : public Dispatcher<int, Payload::payload_ptr>
{
	/// Constants:
public:
	static const uint8_t SPANNING_TREE_QUEUE = 0;
	static const uint8_t BROADCAST_QUEUE = 1;
	static const uint8_t FORWARDING_QUEUE = 2;
	
public:
	/// Default constructor.
	FromPinkAgentDispatcher();

	/// Default destructor.
	~FromPinkAgentDispatcher();

public:
	/// Start redirecting network messages
	virtual int run() override;

	/// This method extracts the key from a data stored in the fromQueue
	virtual int extractKey(const std::shared_ptr<Payload> &data) const override;
};

#endif // FROM_PINK_AGENT_DISPTACHER_H