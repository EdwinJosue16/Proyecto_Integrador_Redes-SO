#ifndef Pink_PRODUCER
#define Pink_PRODUCER

#include "CommonTypes.h"
#include "../common/Producer.hpp"
#include "TcpSocketWrapper.h"
#include "./Payloads/PinkPayload.h"
#include <strings.h> //bzero()
#include "Log.hpp"

class PinkProducer : public Producer<payload_ptr>
{
	DISABLE_COPY(PinkProducer);

private:
	TcpSocketWrapper *tcpSocket;
	int portForReceivingToNode;
	std::string ipForReceivingToNode;
	bool keepWorking;
	const int MAX_TCP_MESSAGE_SIZE = 1024;

private:
	///
	void attendRequestFromNode();

	///
	void sendRequestToNetworkLayer(const char* buffer);

public:
	///
	PinkProducer();

	///
	PinkProducer(int portForSendingToNode);

	///
	PinkProducer(int portForSendingToNode, std::string);

	~PinkProducer();

	///
	virtual int run() override;

	///
	inline void setPortForReceivingFromNode(int portForReceivingFromNode)
	{
		this->portForReceivingToNode = portForReceivingFromNode;
		this->ipForReceivingToNode = "127.0.0.1";
		this->tcpSocket = new TcpSocketWrapper(portForReceivingFromNode);
		this->keepWorking=true;
	}

	///
	inline int getPortForReceivingToNode()
	{
		return this->portForReceivingToNode;
	}
};
#endif