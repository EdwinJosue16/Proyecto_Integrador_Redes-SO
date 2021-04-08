#include "ApplicationShortMessagePayload.h"
#include "../Log.hpp"
#include "../CommonTypes.h"
#include <cstring>

ApplicationShortMessagePayload::ApplicationShortMessagePayload()
{
}

ApplicationShortMessagePayload::~ApplicationShortMessagePayload()
{
}

void ApplicationShortMessagePayload::deserialize(const char *buffer)
{
	Log::append("***** Short Message Payload ***** < Deserialization process begins. >");

	char raw[MAX_MESSAGE_SIZE];
	std::memset(raw, '\0', MAX_MESSAGE_SIZE);
	std::memcpy(raw, buffer, this->length(buffer));
	this->message = std::string(raw);

	Log::append("***** Short Message Payload ***** < Deserialized message " + this->message + "> .");
	Log::append("***** Short Message Payload ***** < Deserialization process ends. >");
}

void ApplicationShortMessagePayload::serialize(char *buffer)
{
	Log::append("***** Short Message Payload ***** < Serialization process begins. >");
	std::memcpy(buffer, this->message.c_str(), this->message.size() + 1);
	Log::append("***** Short Message Payload ***** < Serialized message " + this->message + "> .");
	Log::append("***** Short Message Payload ***** < Serialization process ends. >");
}

size_t ApplicationShortMessagePayload::getBytesRepresentationCount()
{
	return this->message.size() + 1;
}

Payload::payload_ptr ApplicationShortMessagePayload::makeDeepCopy()
{
	return std::make_shared<ApplicationShortMessagePayload>();
}

uint8_t ApplicationShortMessagePayload::length(const char *buffer)
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
