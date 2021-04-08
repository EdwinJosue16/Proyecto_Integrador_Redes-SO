#ifndef SPANNING_TREE_PAYLOAD_H
#define SPANNING_TREE_PAYLOAD_H

#include "../../common/Payload.hpp"
#include <vector>

class SpanningTreePayload : public Payload
{
	/// Constants.
public:
	/// Spanning tree payload types.
	static const uint8_t SPANNING_TREE_REQUEST = 2;
	static const uint8_t SPANNING_TREE_YES_ANSWER = 3;
	static const uint8_t SPANNING_TREE_NO_ANSWER = 4;
	static const uint8_t SPANNING_TREE_ANSWER_ACK = 5;
	static const uint8_t SPANNING_TREE_CLOSING_ACK = 6;
	static const uint8_t SPANNING_TREE_DISCONNECTION = 7;

	/// Offset used in the serialization and deserialization process.
	static const uint8_t TYPE_OFFSET = 0;
	static const uint16_t SEQUENCE_NUMBER_OFFEST = 1;
	static const uint16_t REQUEST_NUMBER_OFFEST = 3;

	static const uint8_t SOURCE_ID_INDEX = 0;
	static const uint8_t DESTINATION_ID_INDEX = 1;
	static const uint8_t SPANNING_TREE_MESSAGE_INDEX = 2;

	static const uint8_t TYPE_INDEX = 0;
	static const uint8_t SEQUENCE_NUMBER_INDEX = 1;
	static const uint8_t REQUEST_NUMBER_INDEX = 2;

	static const uint8_t COLON_DELIMITER_POSITION = 3;

	// Class members. (They were made public to ease object construction.)
public:
	//// Type is inherited from base class Payload.

	/// Sequence number.
	uint16_t sequenceNumber;

	/// Request number.
	uint16_t resquestNumber;

	/// This field is used to determine to wchich immediate neighbor the spanning tree payload must be sent.
	/// This field is not serialized nor deserialized, but it is used in communication with pink node.
	uint16_t destinationId;

	/// This field is used to determine the immediate source id who sent the spanning tree payload.
	/// This field is part of the message sent to pink node from the spanning tree module that processes payloads comming from other nodes.
	uint16_t sourceId;

public:
	/// Default constructor.
	SpanningTreePayload();

	/// Default destructor.
	~SpanningTreePayload();

	/// Inherited must implement methods from base class Payload.
public:
	/// It process a given buffer and initializes *this attributes via casting.
	virtual void deserialize(const char *buffer) override;

	/// It extracts *this attributes and stored them on buffer.
	virtual void serialize(char *buffer) override;

	/// It returns the total bytes that *this will need for serialization. It it recursively called on *this payload.
	virtual size_t getBytesRepresentationCount() override;

	/// It makes a deep copy of *this.
	virtual Payload::payload_ptr makeDeepCopy();

	/// Utilities for pink node communication.
public:
	/// It builds a comma separeted string with *this information. The string is marked with spanning tree type at the begining.
	std::string toString();

	void buildFromString(const std::string &source);
};

#endif // SPANNING_TREE_PAYLOAD_H