#include "PinkConsumer.h"
#include <cstring>

static const int BUFFER_SIZE = 256;

PinkConsumer::PinkConsumer()
{
	this->tcpSocket = nullptr;
	this->portForSendingToNode = 12345;		//DEFAULT PORT
	this->ipForSendingToNode = "127.0.0.1"; //localhost is defult "IP"
}

PinkConsumer::PinkConsumer(int portForSendingToNode)
{
	this->portForSendingToNode = portForSendingToNode;
	this->ipForSendingToNode = "127.0.0.1";
	this->tcpSocket = new TcpSocketWrapper(portForSendingToNode);
}

PinkConsumer::PinkConsumer(int portForSendingToNode, std::string ipForSendingToNode)
{
	this->portForSendingToNode = portForSendingToNode;
	this->ipForSendingToNode = ipForSendingToNode;
	this->tcpSocket = new TcpSocketWrapper(portForSendingToNode, ipForSendingToNode);
}

PinkConsumer::~PinkConsumer()
{
	if (tcpSocket != nullptr)
	{
		delete tcpSocket;
	}
	tcpSocket = nullptr;
}

int PinkConsumer::run()
{
	if (tcpSocket != nullptr)
	{

		tcpSocket->build();
		Log::append("***** PinkConsumer          ***** <  tcpSocket were built, time to consume forever >");
		this->consumeForever();
		return EXIT_SUCCESS;
		Log::append("***** PinkConsumer          ***** <  has finished>");
	}
	else
	{
		Log::append("***** PinkConsumer          ***** <  tcpSocket is nullptr. >");
		return EXIT_FAILURE;
	}
}

void PinkConsumer::consume(const Payload::payload_ptr &payload)
{
	auto pinkPayload = std::static_pointer_cast<PinkPayload>(payload);
	Log::append("***** PinkConsumer          ***** <  A request for pink node is" + pinkPayload->pinkMessage + " .>");

	// It stops TCP protocol to concatenate small packages. The extra padding characters '$' are added just to fill out the space;
	// in the receiving side those characters will be removed and the original message will be extracted.
	char buffer[BUFFER_SIZE];
	std::memset(buffer, '$', BUFFER_SIZE);
	std::memcpy(buffer, pinkPayload->pinkMessage.c_str(), pinkPayload->pinkMessage.size());

	send(tcpSocket->getFileDescriptor(),
		 buffer,
		 BUFFER_SIZE,
		 0);
}
