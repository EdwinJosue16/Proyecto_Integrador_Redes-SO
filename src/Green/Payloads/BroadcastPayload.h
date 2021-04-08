#ifndef BROADCAST_PAYLOAD_H
#define BROADCAST_PAYLOAD_H

#include "../../common/Payload.hpp"

class BroadcastPayload : public Payload
{
	DISABLE_COPY(BroadcastPayload);

	/// Constants.
public:
	/// Broadcast types.
	static const uint8_t BROADCAST_MESSAGE_TYPE = 1;
	static const uint8_t REACHABILITY_TYPE = 2;

	/// Offsets used to serialize and deserialize.
	static const uint8_t TYPE_OFFSET = 0;
	static const uint8_t SOURCE_ID_OFFSET = 1;
	static const uint8_t PAYLOAD_LENGTH_OFFSET = 3;
	static const uint8_t PAYLOAD_OFFSET = 5;

	static const uint8_t BROADCAST_SENDING_SIDE = 0;
	static const uint8_t BROADCAST_RECEIVING_SIDE = 1;

	// Class members. (They were made public to ease object construction.)
public:
	/// Type is inherited from the base Payload class.

	/// Id of the node that generated the original broadcast message.
	uint16_t sourceId;

	// The payload length.
	uint16_t payloadLength;

	/// Payload is inherited from the base Payload class.

	uint8_t broadcastSide;

	// This field is used in broadcast receiving side module to avoid sending the payload to the node
	// who just sent the payload.
	uint16_t immediateSource;

public:
	/// Default constructor.
	BroadcastPayload();

	/// Default destructor.
	~BroadcastPayload();

	/// Inherited methods:

public:
	/// It process a given buffer and initializes *this attributes via casting.
	virtual void deserialize(const char *buffer) override;

	/// It extracts *this attributes and stored them on buffer.
	virtual void serialize(char *buffer) override;

	/// It returns the total bytes that *this will need for serialization. It it recursively called on *this payload.
	virtual size_t getBytesRepresentationCount() override;

	/// It makes a deep copy of *this.
	virtual Payload::payload_ptr makeDeepCopy();

private:
	/// It is used to print the type on Log.
	std::string stringType();
};

#endif // BROADCAST_PAYLOAD_H