#include "BroadcastMessagePayload.h"
#include "../Log.hpp"
#include "../CommonTypes.h"

#include <cstring> // memcpy

BroadcastMessagePayload::BroadcastMessagePayload()
{
}

BroadcastMessagePayload::~BroadcastMessagePayload()
{
}

void BroadcastMessagePayload::deserialize(const char *buffer)
{
	Log::append("***** Broadcast Message       ***** < Deserialization process begins. >");

	char raw[MAX_MESSAGE_SIZE];
	std::memset(raw, '\0', MAX_MESSAGE_SIZE);
	std::memcpy(raw, buffer, this->length(buffer));

	this->message = std::string(raw);

	Log::append("***** Broadcast Message       *****  < Deserialized message: " + this->message + " >");
	Log::append("***** Broadcast Message       ***** < Deserialization process ends. >");
}

void BroadcastMessagePayload::serialize(char *buffer)
{
	Log::append("***** Broadcast Message       ***** < Serialization process begins. >");

	std::memcpy(buffer, this->message.c_str(), this->message.size() + 1);

	Log::append("***** Broadcast Message       ***** < Serialized message: " + this->message + " >");
	Log::append("***** Broadcast Message       ***** < Serialization process ends. >");
}

size_t BroadcastMessagePayload::getBytesRepresentationCount()
{
	return this->message.size() + 1;
}

Payload::payload_ptr BroadcastMessagePayload::makeDeepCopy()
{
	Log::append("***** Broadcast Message       ***** < Deep copy process begins. >");

	auto broadcastMessagePayloadCopy = std::make_shared<BroadcastMessagePayload>();
	broadcastMessagePayloadCopy->message = this->message;

	Log::append("***** Broadcast Message       ***** < Deep copy process ends. >");

	return broadcastMessagePayloadCopy;
}

uint8_t BroadcastMessagePayload::length(const char *buffer)
{
	uint8_t messageLength = 0;
	uint8_t index = 0;

	while (buffer[index] != '\0')
	{
		++messageLength;
		++index;
	}

	return messageLength;
}
