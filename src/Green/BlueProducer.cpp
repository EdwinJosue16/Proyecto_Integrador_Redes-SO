#include "BlueProducer.h"
#include "./Payloads/LinkLayerPayload.hpp"
#include "./Payloads/NetworkPayload.hpp"
#include "./Payloads/ApplicationPayload.h"

#include <algorithm>
#include <sstream>

#define INPUT_PIPE_MESSAGE_SIZE 256
#define DEFAULT_VALUE -100
#define STOP_CONDITION "0:end"

BlueProducer::BlueProducer()
{
}

BlueProducer::~BlueProducer()
{
}

void BlueProducer::listenBlueNode()
{
	bool keepWorking = true;
	std::string message;

	while (keepWorking)
	{
		//Reads message from pipe
		receiveMessageFromBlueNode(message);
		Log::append("***** Blue Producer           ***** < Received:" + message + ">");
		this->produce(buildPackage(message));
		Log::append("***** Blue Producer           ***** < An application payload has been sent to the network layer >");
		message = "";
		keepWorking = message != STOP_CONDITION;
	}
}

bool BlueProducer::isABroadcastMessage(std::string &message)
{
	if (message.find("BROADCAST") != std::string::npos)
		return true;

	return false;
}

void BlueProducer::receiveMessageFromBlueNode(std::string &message)
{
	char messageFromPipe[INPUT_PIPE_MESSAGE_SIZE];
	std::memset(messageFromPipe, '\0', INPUT_PIPE_MESSAGE_SIZE);

	read(this->readEndWrightPipe, messageFromPipe, INPUT_PIPE_MESSAGE_SIZE);
	message = std::string(messageFromPipe);
}

payload_ptr BlueProducer::buildPackage(std::string formattedMesage)
{
	// It checks whether the message has the broadcast or random modificator. By default is forwarding.

	// It removes all the occurrences of '\'' and '\t' characters from message.
	formattedMesage.erase(std::remove(formattedMesage.begin(), formattedMesage.end(), '\''), formattedMesage.end());
	formattedMesage.erase(std::remove(formattedMesage.begin(), formattedMesage.end(), '\t'), formattedMesage.end());
	formattedMesage.erase(std::remove(formattedMesage.begin(), formattedMesage.end(), '-'), formattedMesage.end());

	int delimiterPosition = formattedMesage.find(":");

	uint16_t finalDestinationId = 0;
	auto applicationPayload = std::make_shared<ApplicationPayload>();

	if (isABroadcastMessage(formattedMesage))
	{
		applicationPayload->type = BROADCAST_TYPE;
		finalDestinationId = EVERYONE;
	}
	else
	{
		applicationPayload->type = FORWARDING_TYPE;
		finalDestinationId = (uint16_t)extractFinalDestinationFromMessage(formattedMesage);
	}

	// It extracts the actual message from the mesage received from blue node.
	std::string message = formattedMesage.substr(delimiterPosition + 1, formattedMesage.length() - 1);
	applicationPayload->destinationId = finalDestinationId;
	applicationPayload->sourceId = this->greenNodeId;
	applicationPayload->message = message;

	return applicationPayload;
}

uint16_t BlueProducer::extractFinalDestinationFromMessage(std::string message)
{
	std::stringstream strintToTokenize(message);
	std::vector<std::string> tokens;
	std::string intermediate;

	while (getline(strintToTokenize, intermediate, ':'))
	{
		tokens.push_back(intermediate);
	}

	std::string id;
	for (size_t i = 0; i < tokens.size(); i++)
	{
		id += tokens[i];
	}

	return (int16_t)std::stoi(id);
}

int BlueProducer::run()
{
	this->listenBlueNode();
	return EXIT_SUCCESS;
}
