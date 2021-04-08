#ifndef LINK_LAYER_PAYLOAD
#define LINK_LAYER_PAYLOAD

#include "../../common/Payload.hpp"
#include "NetworkPayload.hpp"
#include "ForwardingPayload.hpp"

class LinkLayerPayload : public Payload
{
    DISABLE_COPY(LinkLayerPayload);

    // Class members. (They were made public to ease object construction.)
public:
    uint16_t immediateDestinationId; // This one is used for inner implementation and redirection.

    uint16_t finalDestinationId; // This is the one that is serialized and sent trough the socket.
    uint16_t sourceId;
    uint16_t payloadLength;

    /// Constants.
public:
    const uint8_t FINAL_DESTINATION_ID_OFFSET = 1;
    const uint8_t IMMEDIATE_DESTINATION_ID_OFFSET = 3;
    const uint8_t PAYLOAD_LENGTH_OFFSET = 5;
    const uint8_t PAYLOAD_OFFSET = 7;

    static const uint8_t HEARTBEAT_TYPE = (uint8_t)1;
    static const uint8_t NETWORK_TYPE = (uint8_t)2;

public:
    /// Default constructor.
    LinkLayerPayload();

    /// Default destructor.
    ~LinkLayerPayload();

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