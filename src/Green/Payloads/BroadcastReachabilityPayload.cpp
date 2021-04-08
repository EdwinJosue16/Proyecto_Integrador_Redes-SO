
#include "BroadcastReachabilityPayload.h"
#include "../Log.hpp"

static const std::string BROADCAST_REACHABILITY_UPDATE_MARK = "BRU";

BroadcastReachabilityPayload::BroadcastReachabilityPayload()
{
}

BroadcastReachabilityPayload::~BroadcastReachabilityPayload()
{
}

void BroadcastReachabilityPayload::deserialize(const char *buffer)
{
	Log::append("***** Broadcast Reachability  ***** < Deserialization process begins. >");

	this->numberOfNeighbors = *(const uint16_t *)(buffer + NUMBER_OF_NEIGHBORS_OFFSET);

	/*----------------------------------------------------------------------------------------------------

	Here is a brief explanation on how the pointer arithmetics works to extract the tuples from the buffer.

	It is known that to store the number of neighbors, neighbor id, and distance, 2, 2, and 1 one bytes are required, respectively. 

	Lets assume that for the reachability payload the number of neighbors is 3. That means that there will be 3 tuples of neighbors in the
	form (neighbors-id, distance).

	Hence, the memory layout will look as follows:

	Byte 0  Byte 1    Byte 2  Byte 3  Byte 4    Byte 5  Byte 6  Byte 7    Byte 8  Byte 9  Byte 10
	+-----+ +-----+   +-----+ +-----+ +-----+   +-----+ +-----+ +-----+   +-----+ +-----+ +-----+
	|     | |     |   |     | |     | |     |   |     | |     | |     |   |     | |     | |     |
	+-----+ +-----+   +-----+ +-----+ +-----+   +-----+ +-----+ +-----+   +-----+ +-----+ +-----+
	+  Neighbors  +   +    Id #1    + + D1  +   +    Id #2    + + D2  +   +    Id #3    + + D3  +   

	Where "Neighbors", "ID #N", and "DN" stand for the total number of neighbors, neighbor id, and distance to that neighbors, respectively.

	First, in order to start extracting the tuples, it is needed to reach the first byte of the first neighbor tuple.
	To do that, a pointer to byte number 2 is needed. That is why (buffer + FIRST_NEIGHBOR_OFFSET) is used to initialize the pointer
	that will be used to move across the buffer.

	Second, to reach the neighbor distance tuple from the first byte of each tuple an increment of 2 bytes from the current pointer offset is needed. 
	That is why (pointer-offset + NEIGHBOR_DISTANCE_OFFSET) is used to access such byte.

	Finally, it can be seen that bytes 2, 5, and 8 point the begining of each tuple. If you pay close attention, you will notice that to reach
	the next tuple from the current one an increment of 3 bytes in the pointer offset is needed. That is why after pushing the newly created
	tuple to the vector the pointer offsset is incremented in NEXT_NEIGHBOR_ID_OFFSET.

	----------------------------------------------------------------------------------------------------*/

	const char *currentOffset = (const char *)(buffer + FIRST_NEIGHBOR_OFFSET);

	for (size_t i = 0; i < this->numberOfNeighbors; i++)
	{
		ReachabilityTuple neighborTuple;

		neighborTuple.neighborId = *(const uint16_t *)(currentOffset);
		neighborTuple.neighborDistance = *(const uint8_t *)(currentOffset + NEIGHBOR_DISTANCE_OFFSET);

		this->reachability.push_back(neighborTuple);

		currentOffset += NEXT_NEIGHBOR_ID_OFFSET;
	}

	Log::append("***** Broadcast Reachability  ***** < Deserialization process ends. >");
}

void BroadcastReachabilityPayload::serialize(char *buffer)
{
	Log::append("***** Broadcast Reachability  ***** < Serialization process begins. >");

	*(uint16_t *)(buffer + NUMBER_OF_NEIGHBORS_OFFSET) = this->numberOfNeighbors;

	// It stores each reachability tuple in the buffer.

	char *currentOffset = (char *)(buffer + FIRST_NEIGHBOR_OFFSET);

	for (auto &neighborTuple : this->reachability)
	{
		*(uint16_t *)(currentOffset) = neighborTuple.neighborId;
		*(uint8_t *)(currentOffset + NEIGHBOR_DISTANCE_OFFSET) = neighborTuple.neighborDistance;
		currentOffset += NEXT_NEIGHBOR_ID_OFFSET;
	}

	Log::append("***** Broadcast Reachability  ***** < Serialization process ends. >");
}

size_t BroadcastReachabilityPayload::getBytesRepresentationCount()
{
	// Each reachability tuple is formed by 3 bytes; 2 bytes for neighbor id and 1 byte for neighbor distance. Thus, the total bytes count is 2 bytes for the number
	// neighbors plus the number of neighbors by 3.

	return sizeof(this->numberOfNeighbors) + (size_t)(BYTES_IN_EACH_TUPLE * this->numberOfNeighbors);
}

Payload::payload_ptr BroadcastReachabilityPayload::makeDeepCopy()
{
	// To be implemented.
	return std::make_shared<BroadcastReachabilityPayload>();
}

std::string BroadcastReachabilityPayload::toString()
{
	/*
		Lets assume that broadcast source id is 150, that number of neighbors is 3 and that the reachability vector looks like this: (170,1), (145, 2), (200,3).
		This means that 170, 145, and 200 are immediate neighbors of 150 and that each on of them is at 1, 2, and 3 units of distance, respectively.

		The string representation of *this will look as: BRU:150*3;170,1,145,2,200,3
		In general it will look as: BRU:broadcast-source-id*number-of-neighbors;neighbor-id-1,distance-1, neighbor-id-2, distance-2, ....., neighbor-id-N, distance-N
	*/

	std::string representation =
		BROADCAST_REACHABILITY_UPDATE_MARK + ":" + std::to_string(this->broadcastSourceId) + "*" + std::to_string(this->numberOfNeighbors) + ";";

	for (auto &tuple : this->reachability)
	{
		representation += std::to_string(tuple.neighborId) + "," + std::to_string(tuple.neighborDistance) + ",";
	}

	// After filling up the string representation a non desired comma character is at the end, so it is removed.
	representation.pop_back();

	return representation;
}

void BroadcastReachabilityPayload::initializeReachability(neighbors_identity_t &neighborsIdentity)
{
	for (auto &neighbor : neighborsIdentity)
	{
		ReachabilityTuple tuple;
		tuple.neighborId = neighbor.id;
		//tuple.neighborDistance = neighbor.distance;

		this->reachability.push_back(tuple);
	}
}
