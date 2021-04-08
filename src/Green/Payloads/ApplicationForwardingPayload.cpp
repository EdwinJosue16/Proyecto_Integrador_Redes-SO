#include "ApplicationForwardingPayload.h"
#include "../Log.hpp"
#include "ApplicationShortMessagePayload.h"

ApplicationForwardingPayload::ApplicationForwardingPayload()
{
}

ApplicationForwardingPayload::~ApplicationForwardingPayload()
{
}

void ApplicationForwardingPayload::deserialize(const char *buffer)
{
	Log::append("***** Application Forwarding Payload ***** < Deserialization process begins. >");

	this->type = *(const uint8_t *)(buffer + TYPE_OFFEST);
	this->payloadLength = *(const uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET);

	if (this->type == SHORT_MESSAGE_TRANSMISSION)
	{
		this->payload = std::make_shared<ApplicationShortMessagePayload>();
	}
	else if (this->type == SHORT_MESSAGE_TRANSMISSION)
	{
		//this->payload = std::make_shared<ReliableTransmissionPayload>();
	}

	Log::append("***** Application Forwarding Payload ***** < Deserialized type: " + this->stringType() + " >.");
	Log::append("***** Application Forwarding Payload ***** < Deserialized payload length: " + std::to_string(this->payloadLength) + " >.");
	Log::append("***** Application Forwarding Payload ***** < Deserialization process ends. >");

	this->payload->deserialize(buffer + PAYLOAD_OFFSET);
}

void ApplicationForwardingPayload::serialize(char *buffer)
{
	Log::append("***** Application Forwarding Payload ***** < Serialization process begins. >");

	*(uint8_t *)(buffer + TYPE_OFFEST) = this->type;
	*(uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET) = this->payloadLength;

	Log::append("***** Application Forwarding Payload ***** < Serialized type: " + std::to_string(this->type) + " >.");
	Log::append("***** Application Forwarding Payload ***** < Serialized payload length: " + std::to_string(this->payloadLength) + " >.");
	Log::append("***** Application Forwarding Payload ***** < Serialization process ends. >");
	this->payload->serialize(buffer + PAYLOAD_OFFSET);
}

size_t ApplicationForwardingPayload::getBytesRepresentationCount()
{
	return sizeof(this->type) + sizeof(this->payloadLength) + this->payload->getBytesRepresentationCount();
}

Payload::payload_ptr ApplicationForwardingPayload::makeDeepCopy()
{
	return std::make_shared<ApplicationForwardingPayload>();
}
