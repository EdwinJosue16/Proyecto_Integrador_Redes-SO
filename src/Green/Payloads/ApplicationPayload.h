#ifndef APPLICATION_PAYLOAD_H
#define APPLICATION_PAYLOAD_H

#include "../../common/Payload.hpp"

const uint8_t FORWARDING_TYPE = 0;
const uint8_t BROADCAST_TYPE = 1;
const int8_t EVERYONE = 0;
const int8_t LIVING_NEIGHBORS = 4;

class ApplicationPayload : public Payload
{
	// Class members. (They were made public to ease object construction.)
public:
	uint8_t destinationId;
	uint8_t sourceId;
	std::string message;

public:
	/// Default constructo.
	ApplicationPayload();
	/// Default destructor.
	~ApplicationPayload();

	/// Inherited methods that will not be used. An empty implementation is provided to avoid compilation errors.
public:
	/// It process a given buffer and initializes *this attributes via casting.
	virtual void deserialize(const char *buffer) override;

	/// It extracts *this attributes and stored them on buffer.
	virtual void serialize(char *buffer) override;

	/// It returns the total bytes that *this will need for serialization. It it recursively called on *this payload.
	virtual size_t getBytesRepresentationCount() override;

	/// It makes a deep copy of *this.
	virtual Payload::payload_ptr makeDeepCopy();
};

#endif // APPLICATION_PAYLOAD_H