#ifndef FROM_LINKLAYER_DISPATCHER_H
#define FROM_LINKLAYER_DISPATCHER_H

#include "../../common/Dispatcher.hpp"
#include "../../common/Payload.hpp"

typedef uint8_t Key;
static const uint8_t FORWARDING_QUEUE = 0;
static const uint8_t BROADCAST_QUEUE = 1;
static const uint8_t SPANNING_TREE_QUEUE = 2;
class FromLinkLayerDispatcher : public Dispatcher<Key, Payload::payload_ptr>
{
public:
	/// Constants.
public:
	/// Default constructor.
	FromLinkLayerDispatcher();

	/// Default destructor();
	~FromLinkLayerDispatcher();

	/// Inherited methods.
public:
	virtual Key extractKey(const Payload::payload_ptr &data) const;

	virtual int run();
};

#endif // FROM_LINKLAYER_DISPATCHER_H