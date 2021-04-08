#include "BlueConsumer.h"
#include "../common/Consumer.hpp"
#include "Payloads/ForwardingPayload.hpp"
#include "Payloads/NetworkPayload.hpp"
#include "Payloads/ApplicationPayload.h"
#include "CommonTypes.h"

#include <unistd.h> //pipe
#include <memory>	//static_pointer_cast

BlueConsumer::BlueConsumer()
{
}

BlueConsumer::~BlueConsumer()
{
}

int BlueConsumer::run()
{
	Log::append("***** Blue Consumer ***** < has started >");
	// Start the forever loop to consume all the messages that arrive
	this->consumeForever();

	// If the forever loop finished, no more messages will arrive
	return EXIT_SUCCESS;
}

void BlueConsumer::consume(const payload_ptr &package)
{
	// It expects to receive a application payload with the message.

	Log::append("***** Blue Consumer           ***** < A payload was received from the dispatcher.");

	auto applicationPayload = std::static_pointer_cast<ApplicationPayload>(package);

	std::string message(applicationPayload->message);

	if (applicationPayload->type == BROADCAST_TYPE)
	{
		message += " Enviado mediante broadcast por el vecino #" + std::to_string(applicationPayload->sourceId) + "\0";
	}
	else if (applicationPayload->type == LIVING_NEIGHBORS)
	{
		Log::append("***** Blue Consumer           ***** < A payload with living neighbors was receive.");
	}
	else
	{
		message += " -> Enviado por el vecino #" + std::to_string(applicationPayload->sourceId) + "\0";
	}

	write(this->fileDescriptorWriteEndPipe, message.c_str(), message.length());

	Log::append("***** Blue Consumer           ***** < A message was sent to blue node >");
}
