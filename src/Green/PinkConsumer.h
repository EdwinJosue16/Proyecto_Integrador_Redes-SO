#ifndef PINK_CONSUMER
#define PINK_CONSUMER

#include <string>

#include "../common/Consumer.hpp"
#include "TcpSocketWrapper.h"
#include "Log.hpp"
#include "../common/Payload.hpp"
#include "./Payloads/PinkPayload.h"

class PinkConsumer : public Consumer<Payload::payload_ptr>
{
	DISABLE_COPY(PinkConsumer);

private:
	TcpSocketWrapper *tcpSocket;
	int portForSendingToNode;
	std::string ipForSendingToNode;

public:
	///
	PinkConsumer();

	///
	PinkConsumer(int portForSendingToNode);
	///
	PinkConsumer(int portForSendingToNode, std::string);

	///
	~PinkConsumer();

	/// It consumes payload and after makes a cast to pink payloads in order to get "serialized info into string"
	virtual void consume(const Payload::payload_ptr &pack) override;

	///
	virtual int run() override;

	///
	inline void setPortForSendingToNode(int portForSendingToNode)
	{
		this->portForSendingToNode = portForSendingToNode;
		this->ipForSendingToNode = "127.0.0.1";
		this->tcpSocket = new TcpSocketWrapper(portForSendingToNode);
	}

	///
	inline int getPortForSendingToNode()
	{
		return this->portForSendingToNode;
	}
};

#endif