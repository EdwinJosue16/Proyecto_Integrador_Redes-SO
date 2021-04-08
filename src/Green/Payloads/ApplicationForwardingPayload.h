#ifndef APPLICATION_FORWARDING_PAYLOAD_H
#define APPLICATION_FORWARDING_PAYLOAD_H

#include "../../common/Payload.hpp"

class ApplicationForwardingPayload : public Payload
{
	DISABLE_COPY(ApplicationForwardingPayload);
	// Class members. (They were made public to ease object construction.)
public:
	/// Type is inherited from base class.
	uint16_t payloadLength;
	/// Payload is inherited from base class.

	/// Constants
public:
	static const uint8_t SHORT_MESSAGE_TRANSMISSION = 1;
	static const uint8_t RELIABLE_TRANSMISSION = 2;

	/// Deserialization - Serialization offsets.
	static const uint8_t TYPE_OFFEST = 0;
	static const uint8_t PAYLOAD_LENGTH_OFFSET = 1;
	static const uint8_t PAYLOAD_OFFSET = 3;

public:
	/// Default constructor.
	ApplicationForwardingPayload();

	/// Default destructor.
	~ApplicationForwardingPayload();

	/// It process a given buffer and initializes *this attributes via casting.
	virtual void deserialize(const char *buffer) override;

	/// It extracts *this attributes and stored them on buffer.
	virtual void serialize(char *buffer) override;

	/// It returns the total bytes that *this will need for serialization. It it recursively called on *this payload.
	virtual size_t getBytesRepresentationCount() override;

	/// It makes a deep copy of *this.
	virtual Payload::payload_ptr makeDeepCopy();
};

#endif // APPLICATION_FORWARDING_PAYLOAD_H