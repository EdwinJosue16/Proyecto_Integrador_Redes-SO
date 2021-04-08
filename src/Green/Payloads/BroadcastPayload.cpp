#include "BroadcastPayload.h"
#include "BroadcastMessagePayload.h"
#include "BroadcastReachabilityPayload.h"

#include "../Log.hpp"

BroadcastPayload::BroadcastPayload()
{
}

BroadcastPayload::~BroadcastPayload()
{
}

void BroadcastPayload::deserialize(const char *buffer)
{
	Log::append("***** Broadcast Payload       ***** < Deserialization process begins. >");

	this->type = *(const uint8_t *)(buffer + TYPE_OFFSET);

	this->sourceId = *(const uint16_t *)(buffer + SOURCE_ID_OFFSET);

	this->payloadLength = *(const uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET);

	Log::append("***** Broadcast Payload       ***** < Deserialized payload type: " + stringType() + " >");
	Log::append("***** Broadcast Payload       ***** < Deserialized source id: " + std::to_string(this->sourceId) + " >");
	Log::append("***** Broadcast Payload       ***** < Deserialized payload length: " + std::to_string(this->payloadLength) + " >");
	Log::append("***** Broadcast Payload       ***** < Deserialization process ends. >");

	if (this->type == BROADCAST_MESSAGE_TYPE)
	{
		this->payload = std::make_shared<BroadcastMessagePayload>();
	}
	else if (this->type == REACHABILITY_TYPE)
	{
		this->payload = std::make_shared<BroadcastReachabilityPayload>();
	}

	this->payload->deserialize(buffer + PAYLOAD_OFFSET);
}

void BroadcastPayload::serialize(char *buffer)
{
	Log::append("***** Broadcast Payload       ***** < Serialization process begins. >");

	*(uint8_t *)(buffer + TYPE_OFFSET) = this->type;

	*(uint16_t *)(buffer + SOURCE_ID_OFFSET) = this->sourceId;

	*(uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET) = this->payloadLength;

	Log::append("***** Broadcast Payload       ***** < Serialized payload type: " + std::to_string(this->type) + " >");
	Log::append("***** Broadcast Payload       ***** < Serialized source id: " + std::to_string(this->sourceId) + " >");
	Log::append("***** Broadcast Payload       ***** < Serialized payload length: " + std::to_string(this->payloadLength) + " >");
	Log::append("***** Broadcast Payload       ***** < Serialization process ends. >");
	Log::append("***** Broadcast Payload       ***** < Serialization process ends. >");

	this->payload->serialize(buffer + PAYLOAD_OFFSET);
}

size_t BroadcastPayload::getBytesRepresentationCount()
{
	return sizeof(this->type) + sizeof(this->sourceId) + sizeof(this->payloadLength) + this->payload->getBytesRepresentationCount();
}

Payload::payload_ptr BroadcastPayload::makeDeepCopy()
{
	Log::append("***** Broadcast Payload       ***** < Deep copy process begins. >");

	auto broadcastPayloadCopy = std::make_shared<BroadcastPayload>();

	broadcastPayloadCopy->type = this->type;
	broadcastPayloadCopy->sourceId = this->sourceId;
	broadcastPayloadCopy->payloadLength = this->payloadLength;

	Log::append("***** Broadcast Payload       ***** < Deep copy process ends. >");
	
	broadcastPayloadCopy->payload = this->payload->makeDeepCopy();
	
	return broadcastPayloadCopy;
}

std::string BroadcastPayload::stringType()
{
	char deserializedType[3];
	sprintf(deserializedType, "%02X", this->type);
	return std::string(deserializedType);
}
