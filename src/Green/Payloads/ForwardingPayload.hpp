#ifndef FORWARDING_PAYLOAD
#define FORWARDING_PAYLOAD

#include "../../common/Payload.hpp"
uint16_t const MAX_MESSAGE_SIZE = 200;

class ForwardingPayload : public Payload
{
    DISABLE_COPY(ForwardingPayload);

    // Class members. (They were made public to ease object construction.)
public:
    // This one is used for inner implementations details, and it is not transmitted trough the network on a forwarding payload.
    uint16_t destinationId;

    uint16_t sourceId;
    uint16_t payloadLength;

    /// Constants
public:
    const uint8_t PAYLOAD_LENGTH_OFFSET = 2;
    const uint8_t PAYLOAD_OFFSET = 4;

public:
    /// Default constructor.
    ForwardingPayload();
	
    /// Default destructor.
    ~ForwardingPayload();

    /// It process a given buffer and initializes *this attributes via casting.
    virtual void deserialize(const char *buffer) override;

    /// It extracts *this attributes and stored them on buffer.
    virtual void serialize(char *buffer) override;

    /// It returns the total bytes that *this will need for serialization. It it recursively called on *this payload.
    virtual size_t getBytesRepresentationCount() override;

    /// It makes a deep copy of *this.
    virtual Payload::payload_ptr makeDeepCopy();
};
#endif
