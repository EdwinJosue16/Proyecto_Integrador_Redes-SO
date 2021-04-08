#include "SpanningTreePayload.h"
#include "../Log.hpp"

static const std::string SPANNING_TREE_MARK = "SPT";

static const std::string INVALID_STRING_FORMAT =
	"***** Spanning Tree Payload   ***** < "
	"It was impossible to build spanning tree payload from string because"
	"the received string does not follow the required format. >";

SpanningTreePayload::SpanningTreePayload()
{
}

SpanningTreePayload::~SpanningTreePayload()
{
}

void SpanningTreePayload::deserialize(const char *buffer)
{
	Log::append("***** Spanning Tree Payload   ***** < Deserialization process begins. >");

	this->type = *(const uint8_t *)(buffer + TYPE_OFFSET);

	this->sequenceNumber = *(const uint16_t *)(buffer + SEQUENCE_NUMBER_OFFEST);

	this->resquestNumber = *(const uint16_t *)(buffer + REQUEST_NUMBER_OFFEST);

	Log::append("***** Spanning Tree Payload   ***** < Deserialized payload type " + this->stringType() + ". >");
	Log::append("***** Spanning Tree Payload   ***** < Deserialized payload SN " + std::to_string(this->sequenceNumber) + ". >");
	Log::append("***** Spanning Tree Payload   ***** < Deserialized payload RN " + std::to_string(this->resquestNumber) + ". >");

	Log::append("***** Spanning Tree Payload   ***** < Deserialization process ends. >");
}

void SpanningTreePayload::serialize(char *buffer)
{
	Log::append("***** Spanning Tree Payload   ***** < Serialization process begins. >");

	*(uint8_t *)(buffer + TYPE_OFFSET) = this->type;

	*(int16_t *)(buffer + SEQUENCE_NUMBER_OFFEST) = this->sequenceNumber;

	*(int16_t *)(buffer + REQUEST_NUMBER_OFFEST) = this->resquestNumber;

	Log::append("***** Spanning Tree Payload   ***** < Serialized payload type " + this->stringType() + ". >");
	Log::append("***** Spanning Tree Payload   ***** < Serialized payload SN " + std::to_string(this->sequenceNumber) + ". >");
	Log::append("***** Spanning Tree Payload   ***** < Serialized payload RN " + std::to_string(this->resquestNumber) + ". >");

	Log::append("***** Spanning Tree Payload   ***** < Serialization process ends. >");
}

size_t SpanningTreePayload::getBytesRepresentationCount()
{
	return sizeof(this->type) + sizeof(this->sequenceNumber) + sizeof(this->resquestNumber);
}

Payload::payload_ptr SpanningTreePayload::makeDeepCopy()
{
	// To be implemented.
	return std::make_shared<SpanningTreePayload>();
}

std::string SpanningTreePayload::toString()
{
	// In general the format will be SPT:source-id;destination-id;type,SN,RN.

	// Lets assume that: source id is 5, destination id is 10, type is 2, sequence number is 150, and request number is 300.
	// The string representation of *this will look as: SPT:5;10;2,150,300
	// There is no need to send the destination neighbor id because it is the node that has just received the spanning tree payload,
	// and it will be redirected to the associated pink node.

	std::string representation = SPANNING_TREE_MARK + ":" +
								 std::to_string(this->sourceId) + ";" +
								 std::to_string(this->destinationId) + ";" +
								 std::to_string(this->type) + "," +
								 std::to_string(this->sequenceNumber) + "," +
								 std::to_string(this->resquestNumber);
	return representation;
}

void SpanningTreePayload::buildFromString(const std::string &source)
{
	// The expected string must follow the format: SPT:source-id;destination-id;type,SN,RN.
	// Colon delimiter position is always on position 3 whereas semicolon delimiter
	// position can change depending on how many digits the neighbor id has (from 1 to 5).

	if (source.find(SPANNING_TREE_MARK) != std::string::npos)
	{
		// SPT:source-id;destination-id;type,SN,RN.

		// It is a new substring without the SPT mark in the form source-id;destination-id;type,SN,RN
		std::string actualInformation = source.substr(COLON_DELIMITER_POSITION + 1);

		// It returns a new vector where at index 0, index 1, and index 2 are stored source-id, destination-id, (type,SN,RN), respectively.
		auto informationSplittedBySemicolon = this->split(actualInformation, ';');

		this->sourceId = (uint16_t)std::stoi(informationSplittedBySemicolon->at(SOURCE_ID_INDEX));
		this->destinationId = (uint16_t)std::stoi(informationSplittedBySemicolon->at(DESTINATION_ID_INDEX));

		auto spanningTreeMessageSplittedByComma = this->split(informationSplittedBySemicolon->at(SPANNING_TREE_MESSAGE_INDEX), ',');

		this->type = (uint8_t)std::stoi(spanningTreeMessageSplittedByComma->at(TYPE_INDEX));
		this->sequenceNumber = (uint16_t)std::stoi(spanningTreeMessageSplittedByComma->at(SEQUENCE_NUMBER_INDEX));
		this->resquestNumber = (uint16_t)std::stoi(spanningTreeMessageSplittedByComma->at(REQUEST_NUMBER_INDEX));
	}
	else
	{
		Log::append(INVALID_STRING_FORMAT);
	}
}