#ifndef NETWORK_PAYLOAD
#define NETWORK_PAYLOAD

#include "../../common/Payload.hpp"

class NetworkPayload : public Payload
{
    DISABLE_COPY(NetworkPayload);

    // Class members. (They were made public to ease object construction.)
public:
    uint16_t payloadLength;
    uint16_t finalDestination;
    uint16_t immediateDestination;

    /// Constants.
public:
    const uint8_t PAYLOAD_LENGTH_OFFSET = 1;
    const uint8_t PAYLOAD_OFFSET = 3;

    /// Payload types.
public:
    static const uint8_t FORWARDING_TYPE = 1;
    static const uint8_t BROADCAST_TYPE = 2;
    static const uint8_t SPANNING_TREE_TYPE = 3;

public:
    /// Default constructor.
    NetworkPayload();

    /// Default destructor.
    ~NetworkPayload();

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