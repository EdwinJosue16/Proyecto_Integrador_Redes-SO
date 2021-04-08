#include "PinkPayload.h"
#include "../Log.hpp"

static const std::string EMPTY_NEIGHBORS_UPDATE = "EMPTY";

PinkPayload::PinkPayload()
{
}

PinkPayload::~PinkPayload()
{
}

void PinkPayload::deserialize(const char *buffer)
{
	if (buffer != nullptr)
	{
		Log::append("***** Pink Node Payload ***** <Deserialization process mustn't be called on a Pink Payload object. >");
	}
}

void PinkPayload::serialize(char *buffer)
{
	if (buffer != nullptr)
	{
		Log::append("***** Pink Node Payload ***** < Serialization process mustn't be called on a Pink Payload object. >");
	}
}

size_t PinkPayload::getBytesRepresentationCount()
{
	Log::append("***** Pink Node Payload ***** < A Pink Payload object has no bytes representation. >");
	return 0;
}

Payload::payload_ptr PinkPayload::makeDeepCopy()
{
	return std::make_shared<PinkPayload>();
}

std::shared_ptr<std::vector<uint16_t>> PinkPayload::neighborsInSpanningTreeUpdateFromString(std::string &str)
{
	///The str contains something like a "2,3,4,5,1""

	auto table = std::make_shared<std::vector<uint16_t>>();
	if (str.compare(EMPTY_NEIGHBORS_UPDATE) != 0)
	{
		///Then it returns an array like a {"2","3",...,"1"}
		auto tableInStringFormat = this->split(str, ',');

		for (auto &neighbourId : *(tableInStringFormat))
		{
			table->push_back((int16_t)std::stoi(neighbourId));
		}
	}
	else
	{
		Log::append("***** Pink Node Payload ***** < A Pink Payload object has no bytes representation. >");
	}
	return table;
}

void PinkPayload::removeHeaderOfTableInStringFormat()
{
	///In general the message has a header like this (forwarding table): "MARk:,2;3,4;5,6;....;n,m"
	///Or Spanning Tree update like this: "MARk:2,3,4,5,1"
	///Then, it returns an array {"MARk","...."}
	auto splitedTable = this->split(this->pinkMessage, ':');

	///It gets the sencod element in array
	this->pinkMessage = splitedTable->back();
}

std::shared_ptr<std::map<DestinationId, ThroughNeighborId>> PinkPayload::makeForwadingTableFromString(std::string &str)
{
	///Example of forwardingTable in string format: "1,2;3,4;5,6;....;n,m"

	///It returns an array pointer like a {"1,2","3,4","5,6",...,"n,m" }
	auto informationTuples = this->split(str, ';');

	auto forwardingTable = std::make_shared<std::map<DestinationId, ThroughNeighborId>>();

	DestinationId destinationId;
	ThroughNeighborId throughNeighborId;

	/// In this case tuple is an entry of array like a "1,2"
	for (auto &tuple : *informationTuples)
	{
		Log::append("***** Pink Node Payload ***** < Tuple to split:"+ tuple);
		///It returns an array like a {"1","2"} (**)
		auto splittedTuple = this->split(tuple, ',');

		///It returns number: 1 (using example **)
		destinationId = (DestinationId)std::stoi(splittedTuple->front());

		///It returns number: 2 (using example **)
		throughNeighborId = (DestinationId)std::stoi(splittedTuple->back());
		
		Log::append("***** Pink Node Payload ***** < Resulting tuple from FWD string.: >"+ splittedTuple->front()+","+splittedTuple->back());
		forwardingTable->insert({destinationId, throughNeighborId});
	}

	return forwardingTable;
}