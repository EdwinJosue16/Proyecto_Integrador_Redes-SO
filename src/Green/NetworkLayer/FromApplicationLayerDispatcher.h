#ifndef FROM_APPLICATION_LAYER_DISPATCHER_H
#define FROM_APPLICATION_LAYER_DISPATCHER_H

#include "../../common/Dispatcher.hpp"
#include "../../common/Payload.hpp"
#include "../Log.hpp"
#include <memory> // share_ptr

typedef int QueueKey;

class FromApplicationLayerDispatcher : public Dispatcher<QueueKey, std::shared_ptr<Payload>>
{
	/// Constants.
public:
	static const uint8_t FORWARDING_TYPE = 0;
	static const uint8_t BROADCAST_TYPE = 1;

	static const uint8_t FORWARDING_QUEUE = 0;
	static const uint8_t BROADCAST_QUEUE = 1;

public:
	/// Default constructor.
	FromApplicationLayerDispatcher();

	/// Default destructor.
	~FromApplicationLayerDispatcher();

	/// Inherited must implement methods from Thread and Dispatcher classes respectively.
public:
	/// Start redirecting network messages
	virtual int run() override;

	/// This method extracts the key from a data stored in the fromQueue
	virtual int extractKey(const std::shared_ptr<Payload> &data) const override;
};

#endif // FROM_APPLICATION_LAYER_DISPATCHER_H