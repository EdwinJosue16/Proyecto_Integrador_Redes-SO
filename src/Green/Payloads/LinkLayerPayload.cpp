#include "LinkLayerPayload.hpp"
#include "HeartbeatPayload.hpp"
#include "NetworkPayload.hpp"
#include "../Log.hpp"

#include <cstring>

LinkLayerPayload::LinkLayerPayload()
{
}

LinkLayerPayload::~LinkLayerPayload()
{
}

void LinkLayerPayload::deserialize(const char *buffer)
{
    Log::append("***** Link layer payload      ***** < Deserialization process begins. >");

    this->type = *(const uint8_t *)(buffer);

    this->finalDestinationId = *(const uint16_t *)(buffer + FINAL_DESTINATION_ID_OFFSET);

    this->sourceId = *(const uint16_t *)(buffer + IMMEDIATE_DESTINATION_ID_OFFSET);

    this->payloadLength = *(const uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET);

    if (type == HEARTBEAT_TYPE)
    {
        this->payload = std::make_shared<HeartbeatPayload>();
    }
    else if (type == NETWORK_TYPE)
    {
        this->payload = std::make_shared<NetworkPayload>();
    }

    char deserializedType[3];
    sprintf(deserializedType, "%02X", this->type);
    Log::append("***** Link layer payload      ***** < Deserialized payload type: " + std::string(deserializedType) + " >");
    Log::append("***** Link layer payload      ***** < Deserialized source id: " + std::to_string(this->sourceId) + " >");
    Log::append("***** Link layer payload      ***** < Deserialized final destination id: " + std::to_string(this->finalDestinationId) + " >");
    Log::append("***** Link layer payload      ***** < Deserialized payload length: " + std::to_string(this->payloadLength) + " >");
    Log::append("***** Link layer payload      ***** < Deserialization process ends. >");

    this->payload->deserialize(buffer + PAYLOAD_OFFSET);
}

void LinkLayerPayload::serialize(char *buffer)
{
    Log::append("***** Link layer payload      ***** < Serialization process begins. >");

    *(uint8_t *)(buffer) = this->type;

    *(uint16_t *)(buffer + FINAL_DESTINATION_ID_OFFSET) = this->finalDestinationId;

    *(uint16_t *)(buffer + IMMEDIATE_DESTINATION_ID_OFFSET) = this->sourceId;

    *(uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET) = this->payloadLength;

    Log::append("***** Link layer payload      ***** < Serialization payload type: " + std::to_string(this->type) + " >");
    Log::append("***** Link layer payload      ***** < Serialization source id: " + std::to_string(this->sourceId) + " >");
    Log::append("***** Link layer payload      ***** < Serialization final destination id: " + std::to_string(this->finalDestinationId) + " >");
    Log::append("***** Link layer payload      ***** < Serialization payload length: " + std::to_string(this->payloadLength) + " >");
    Log::append("***** Link layer payload      ***** < Serialization process ends. >");

    this->payload->serialize(buffer + PAYLOAD_OFFSET);
}

size_t LinkLayerPayload::getBytesRepresentationCount()
{
    return sizeof(this->type) + sizeof(this->finalDestinationId) + sizeof(this->sourceId) +
           sizeof(this->payloadLength) + this->payload->getBytesRepresentationCount();
}

Payload::payload_ptr LinkLayerPayload::makeDeepCopy()
{
    // To be implemented.
    return std::make_shared<LinkLayerPayload>();
}
