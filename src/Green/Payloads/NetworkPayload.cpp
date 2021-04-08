#include "NetworkPayload.hpp"
#include "ForwardingPayload.hpp"
#include "BroadcastPayload.h"
#include "SpanningTreePayload.h"
#include "../Log.hpp"

#include <cstring>

NetworkPayload::NetworkPayload()
{
}

NetworkPayload::~NetworkPayload()
{
}

void NetworkPayload::deserialize(const char *buffer)
{
    Log::append("***** Network layer payload   ***** < Deserialization process begins. >");

    this->type = *(const uint8_t *)(buffer);
    this->payloadLength = *(const uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET);

    char deserializedType[3];
    sprintf(deserializedType, "%02X", this->type);
    Log::append("***** Network layer payload   ***** < Deserialized payload type: " + std::string(deserializedType) + " >");
    Log::append("***** Network layer payload   ***** < Deserialized payload length: " + std::to_string(this->payloadLength) + " >");

    if (type == FORWARDING_TYPE)
    {
        this->payload = std::make_shared<ForwardingPayload>();
        Log::append("***** Network layer payload   ***** < Payload type is : FORWARDING >");
    }
    else if (type == BROADCAST_TYPE)
    {
        this->payload = std::make_shared<BroadcastPayload>();
        Log::append("***** Network layer payload   ***** < Payload type is : BROADCAST >");
    }
    else if (type == SPANNING_TREE_TYPE)
    {
        this->payload = std::make_shared<SpanningTreePayload>();
        Log::append("***** Network layer payload   ***** < Payload type is : SPANNING TREE >");
    }

    Log::append("***** Network layer payload   *****  < Deserialization process ends. >");

    this->payload->deserialize(buffer + PAYLOAD_OFFSET);
}

void NetworkPayload::serialize(char *buffer)
{
    Log::append("***** Network layer payload   *****  < Serialization process begins. >");

    *(uint8_t *)(buffer) = this->type;

    *(uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET) = this->payloadLength;

    Log::append("***** Network layer payload   ***** < Serialized payload type: " + std::to_string(this->type) + " >");
    Log::append("***** Network layer payload   ***** < Serialized payload length: " + std::to_string(this->payloadLength) + " >");

    Log::append("***** Network layer payload   ***** < Serialization process ends. >");
    this->payload->serialize(buffer + PAYLOAD_OFFSET);
}

size_t NetworkPayload::getBytesRepresentationCount()
{
    return sizeof(this->type) + sizeof(this->payloadLength) + this->payload->getBytesRepresentationCount();
}

Payload::payload_ptr NetworkPayload::makeDeepCopy()
{
    Log::append("***** Network layer payload   ***** < Deep copy process begins. >");

    auto networkPayloadCopy = std::make_shared<NetworkPayload>();

    networkPayloadCopy->type = this->type;
    networkPayloadCopy->payloadLength = this->payloadLength;
    networkPayloadCopy->finalDestination = this->finalDestination;
    networkPayloadCopy->immediateDestination = this->immediateDestination;

    Log::append("***** Network layer payload   ***** < Deep copy process before ends. >");

    networkPayloadCopy->payload = this->payload->makeDeepCopy();

    return networkPayloadCopy;
}
