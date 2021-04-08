#include "HeartbeatPayload.hpp"
#include <cstring>
#include "../Log.hpp"

HeartbeatPayload::HeartbeatPayload()
{
}

HeartbeatPayload::~HeartbeatPayload()
{
}

void HeartbeatPayload::deserialize(const char *buffer)
{
    Log::append("***** Heartbeat payload   ***** < Deserialization process begins. >");

    this->type = *(const uint8_t *)(buffer);

    char deserializedType[3];
    sprintf(deserializedType, "%02X", this->type);

    Log::append("***** Heartbeat payload   ***** < Deserialized payload type: " + std::string(deserializedType) + " >");
    Log::append("***** Heartbeat payload   ***** < Deserialization process ends. >");
}

void HeartbeatPayload::serialize(char *buffer)
{
    Log::append("***** Heartbeat payload   ***** < Serialization process begins. >");

    *(uint8_t *)(buffer) = this->type;

    Log::append("***** Heartbeat payload   ***** < Serialization payload type: " + std::to_string(this->type) + " >");
    Log::append("***** Heartbeat payload   ***** < Serialization process ens. >");
}

size_t HeartbeatPayload::getBytesRepresentationCount()
{
    return sizeof(uint8_t);
}

Payload::payload_ptr HeartbeatPayload::makeDeepCopy()
{
    // To be implemented.
    return std::make_shared<HeartbeatPayload>();
}