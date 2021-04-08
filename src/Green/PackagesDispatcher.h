#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <thread>
#include <vector>
#include <queue>
#include <semaphore.h>
#include <mutex>
#include <map>
#include <cstdint>

#include "TransmittersController.h"
#include "CommonTypes.h"
#include "PackagesDispatcher.h"
#include "../common/Dispatcher.hpp"
#include "Log.hpp"

typedef int QueueID;

class PackagesDispatcher : public Dispatcher<QueueID, payload_ptr>
{
public: // Construction and destruction.
	PackagesDispatcher();
	~PackagesDispatcher();

	/// Constants
public:
	static const uint8_t BLUE_CONSUMER_QUEUE = 1;
	static const uint8_t NETWORK_LAYER_QUEUE = 2;
	static const uint8_t HEARTBEAT_COMPONENT_QUEUE = 3;

public:
	/// Start redirecting network messages
	virtual int run() override;
	/// This method extracts the key from a data stored in the fromQueue
	virtual int extractKey(const payload_ptr &data) const override;
};
#endif
