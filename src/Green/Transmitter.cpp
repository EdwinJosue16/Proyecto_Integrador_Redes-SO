#include "Transmitter.h"
#include "Log.hpp"

Transmitter::Transmitter(int16_t destinationPort = 0, std::string destinationIp = "")
	: destinationPort{destinationPort},
	  destinationIp{destinationIp},
	  talker{destinationIp, std::string(std::to_string(destinationPort))},
	  ready(new Semaphore(0))
{
}

Transmitter::~Transmitter()
{
	delete this->ready;
	this->ready = nullptr;
}

int Transmitter::run()
{
	this->talker.start();
	// It tells the associated visitor that the talker socket was created and it is ready.
	this->ready->signal();

	Log::append("***** Transmitter             ***** <  Transmitter with IP:" + destinationIp + " has started. >");
	this->consumeForever();
	Log::append("***** Transmitter             ***** <  Transmitter with IP:" + destinationIp + " has finished. >");
	return EXIT_SUCCESS;
}

void Transmitter::consume(const payload_ptr &package)
{
	this->talker.sendPackage(package);
}

int16_t Transmitter::getDestinationPort()
{
	Log::append("***** Transmitter             ***** <  This is my destination port: " + std::to_string(destinationPort) + " >");
	return this->destinationPort;
}

std::string &Transmitter::getDestinationIp()
{
	Log::append("***** Transmitter             ***** <  This is my destination IP: " + destinationIp + " >");
	return this->destinationIp;
}

Semaphore *Transmitter::getTransmitterSempahore()
{
	return this->ready;
}
