#ifndef BLUE_PRODUCER
#define BLUE_PRODUCER

#include "CommonTypes.h"
#include "../common/Producer.hpp"
#include <cstring>	//memset strcpy memcpy
#include <unistd.h> //read
#include "Log.hpp"

class BlueProducer : public Producer<payload_ptr>
{
private:
	int readEndWrightPipe;
	int16_t greenNodeId;
	bool keepWorking;

private:
	void receiveMessageFromBlueNode(std::string &readData);
	payload_ptr buildPackage(std::string messageFromPipe);
	uint16_t extractFinalDestinationFromMessage(std::string message);
	void listenBlueNode();
	bool isABroadcastMessage(std::string &message);

public:
	///
	BlueProducer(/* args */);

	///
	~BlueProducer();

	///
	virtual int run() override;

	///
	inline void receiveFileDescriptor(int readEndWrightPipe)
	{
		this->readEndWrightPipe = readEndWrightPipe;
	}

	///

	inline void receiveGreenNodeId(int16_t greenNodeId)
	{
		this->greenNodeId = greenNodeId;
	}
};
#endif