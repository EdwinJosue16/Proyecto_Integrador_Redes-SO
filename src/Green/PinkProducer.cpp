#include "PinkProducer.hpp"

PinkProducer::PinkProducer()
{
	this->tcpSocket = nullptr;
	this->portForReceivingToNode = 12345;	  //DEFAULT PORT
	this->ipForReceivingToNode = "127.0.0.1"; //localhost is defult "IP"
	this->keepWorking = false;
}

PinkProducer::PinkProducer(int portForReceivingToNode)
{
	this->portForReceivingToNode = portForReceivingToNode;
	this->ipForReceivingToNode = "127.0.0.1";
	this->tcpSocket = new TcpSocketWrapper(portForReceivingToNode);
	this->keepWorking = false;
}

PinkProducer::PinkProducer(int portForReceivingToNode, std::string ipForReceivingToNode)
{
	this->portForReceivingToNode = portForReceivingToNode;
	this->ipForReceivingToNode = ipForReceivingToNode;
	this->tcpSocket = new TcpSocketWrapper(portForReceivingToNode, ipForReceivingToNode);
	this->keepWorking = false;
}

PinkProducer::~PinkProducer()
{
	if (tcpSocket != nullptr)
	{
		delete tcpSocket;
	}
	tcpSocket = nullptr;
}

int PinkProducer::run()
{
	if (tcpSocket != nullptr)
	{

		tcpSocket->build();
		Log::append("***** PinkProducer          ***** <  tcpSocket were built, time to attend request from node >");
		this->attendRequestFromNode();
		Log::append("***** PinkProducer          ***** < Has finished >");
		return EXIT_SUCCESS;
	}
	else
	{
		Log::append("***** PinkProducer          ***** <  tcpSocket is nullptr. >");
		return EXIT_FAILURE;
	}
}

void PinkProducer::attendRequestFromNode()
{
	int incomingBytes = 0;
	char buffer[MAX_TCP_MESSAGE_SIZE];
	while (keepWorking)
	{
		bzero(buffer, MAX_TCP_MESSAGE_SIZE);
		incomingBytes = recv(tcpSocket->getFileDescriptor(),
							 buffer,
							 sizeof(buffer),
							 0);

		if (incomingBytes > 0)
		{
			this->sendRequestToNetworkLayer(buffer);
			Log::append("***** PinkProducer          ***** <   The request was send to Network Layer... >");
		}
		else
		{
			Log::append("***** PinkProducer          ***** <   The request from node is corrupted... >");
		}
	}
}

void PinkProducer::sendRequestToNetworkLayer(const char *buffer)
{
	auto pinkPayload = std::make_shared<PinkPayload>();
	pinkPayload->pinkMessage = std::string(buffer);

	if (pinkPayload->pinkMessage.find("SPT") != std::string::npos)
	{
		pinkPayload->type = PinkPayload::SPANNING_TREE_MESSAGE;
	}
	else if (pinkPayload->pinkMessage.find("FWD") != std::string::npos)
	{
		pinkPayload->type = PinkPayload::FORWARDING_TABLE_UPDATE;
	}
	else if (pinkPayload->pinkMessage.find("NSP") != std::string::npos)
	{
		pinkPayload->type = PinkPayload::NEIGHBORS_IN_SPANNING_TREE_TABLE_UPDATE;
	}

	Log::append("***** PinkProducer          ***** <   The request from node is" + pinkPayload->pinkMessage + " >");
	this->produce(pinkPayload);
}