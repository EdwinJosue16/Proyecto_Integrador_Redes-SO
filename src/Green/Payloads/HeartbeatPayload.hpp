#ifndef HEARTBEAT_PAYLOAD
#define HEARTBEAT_PAYLOAD

#include "../../common/Payload.hpp"

class HeartbeatPayload : public Payload
{
    DISABLE_COPY(HeartbeatPayload);

    /// Constants.
public:
    static const uint8_t HEARBEAT_REQUEST = 1;
    static const uint8_t HEARBEAT_ANSWER = 2;

public:
    /// Default constructor.
    HeartbeatPayload();

    /// Default destructor.
    ~HeartbeatPayload();

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