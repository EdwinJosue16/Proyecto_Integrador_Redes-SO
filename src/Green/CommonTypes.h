#ifndef GREEN_NODE_TYPES
#define GREEN_NODE_TYPES
#include "../common/Payload.hpp"
#include "./Payloads/LinkLayerPayload.hpp"
#include "./Payloads/NetworkPayload.hpp"
#include "./Payloads/HeartbeatPayload.hpp"
#include "./Payloads/ForwardingPayload.hpp"
#include "../common/Timestamp.h"

#include <map>
#include <cstdint>
#include <string>
#include <vector>

typedef std::shared_ptr<Payload> payload_ptr;

// ********************************************* Begin Packages Definition *********************************************

#define MESSAGE_SIZE 200


// Forwarding table
typedef uint16_t DestinationId;
typedef uint16_t ThroughNeighborId;

typedef struct green_node_identity_t
{
	int16_t id;
	std::string ip;
	int16_t port;
} green_node_identity_t;

typedef std::vector<green_node_identity_t> neighbors_identity_t;

typedef uint16_t DestinationId;

struct NeighbourStatus
{
	static const uint8_t UNKWON = 2;
	static const uint8_t WAKE_UP = 1;
	static const uint8_t SLEEP = 0;

	int8_t status; //-1,0,1
	Timestamp timestamp;

	NeighbourStatus()
		: status{SLEEP}
	{
	}

	NeighbourStatus &operator=(NeighbourStatus &another)
	{
		status = another.status;
		timestamp = another.timestamp;
		return *this;
	}
};
#endif
