#ifndef REACHABILITY_BROADCAST_PAYLOAD_H
#define REACHABILITY_BROADCAST_PAYLOAD_H

#include "../../common/Payload.hpp"
#include "ReachabilityTuple.h"
#include "../CommonTypes.h"

#include <vector>

class BroadcastReachabilityPayload : public Payload
{
	/// Constants.
public:
	static const uint8_t NUMBER_OF_NEIGHBORS_OFFSET = 0;
	static const uint8_t FIRST_NEIGHBOR_OFFSET = 2;

	static const uint8_t NEXT_NEIGHBOR_ID_OFFSET = 3;
	static const uint8_t NEIGHBOR_DISTANCE_OFFSET = 2;

	static const uint8_t BYTES_IN_EACH_TUPLE = 3;

	// Class members. (They were made public to ease object construction.)
public:
	/// It is the total numbers of tuples in the neighors vectors.
	uint16_t numberOfNeighbors;

	/// Neighbors reachability tuples.
	std::vector<ReachabilityTuple> reachability;

	/// The broadcast source id is needed to determine from who the update comes from because it can be from a far non immediate neighbor node in the network.
	uint16_t broadcastSourceId;

public:
	/// Default constructor.
	BroadcastReachabilityPayload();

	/// Default destructor.
	~BroadcastReachabilityPayload();

	/// Inherited methods.
public:
	/// It process a given buffer and initializes *this attributes via casting.
	virtual void deserialize(const char *buffer) override;

	/// It extracts *this attributes and stored them on buffer.
	virtual void serialize(char *buffer) override;

	/// It returns the total bytes that *this will need for serialization. It it recursively called on *this payload.
	virtual size_t getBytesRepresentationCount() override;

	/// It makes a deep copy of *this.
	virtual Payload::payload_ptr makeDeepCopy();

	/// It builds a comma separeted string with *this information. The string is marked with reachability update type at the begining.
	std::string toString();

	/// It fills the reachability vector with the received neighbors identity.
	void initializeReachability(neighbors_identity_t &neighborsIdentity);
};

#endif // REACHABILITY_BROADCAST_PAYLOAD_H