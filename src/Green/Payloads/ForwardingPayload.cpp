#include "ForwardingPayload.hpp"

#include <cstring> // memcpy
#include <iostream>
#include "../Log.hpp"
#include "ApplicationForwardingPayload.h"

ForwardingPayload::ForwardingPayload()
{
}

ForwardingPayload::~ForwardingPayload()
{
}

void ForwardingPayload::deserialize(const char *buffer)
{
	Log::append("***** Forwarding component    ***** < Deserialization process begins. >");

	this->sourceId = *(const uint16_t *)(buffer);

	this->payloadLength = *(const uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET);

	this->payload = std::make_shared<ApplicationForwardingPayload>();

	Log::append("***** Forwarding component    ***** < Deserialized source id: " + std::to_string(this->sourceId) + " >");
	Log::append("***** Forwarding component    ***** < Deserialized payload length: " + std::to_string(this->payloadLength) + " >");
	Log::append("***** Forwarding component    ***** < Deserialization process ends. >");

	this->payload->deserialize(buffer + PAYLOAD_OFFSET);
}

void ForwardingPayload::serialize(char *buffer)
{
	Log::append("***** Forwarding component    ***** < Serialization process begins. >");

	*(uint16_t *)(buffer) = this->sourceId;

	*(uint16_t *)(buffer + PAYLOAD_LENGTH_OFFSET) = this->payloadLength;

	Log::append("***** Forwarding component    ***** < Serialized source id: " + std::to_string(this->sourceId) + " >");
	Log::append("***** Forwarding component    ***** < Serialized payload length: " + std::to_string(this->payloadLength) + " >");
	Log::append("***** Forwarding component    ***** < Serialization process ends. >");

	this->payload->serialize(buffer + PAYLOAD_OFFSET);
}

size_t ForwardingPayload::getBytesRepresentationCount()
{
	return sizeof(this->sourceId) + sizeof(this->payloadLength) + this->payload->getBytesRepresentationCount();
}

Payload::payload_ptr ForwardingPayload::makeDeepCopy()
{
	return std::make_shared<ForwardingPayload>();
}
