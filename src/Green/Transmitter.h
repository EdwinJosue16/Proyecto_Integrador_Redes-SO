#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <string>

#include "../common/Consumer.hpp"
#include "../common/Semaphore.hpp"
#include "CommonTypes.h"
#include "DgramTalker.h"

class Transmitter : public Consumer<payload_ptr>
{
	DISABLE_COPY(Transmitter);

protected:
	/// ToDo
	int16_t destinationPort;

	/// ToDo
	std::string destinationIp;

	/// Dgram socket to send messages.
	DgramTalker talker;

	// This is used to tell the associated visitor that the socket was successfully created.
	Semaphore* ready;

public:
	/// Constructor.
	Transmitter(int16_t destinationPort, std::string destinationIp);
	/// Destructor.
	~Transmitter();

public:
	/// Consume the messages in its own execution thread.
	virtual int run() override;
	/// Override this method to process any data extracted from the queue.
	virtual void consume(const payload_ptr &package) override;
	///
	int16_t getDestinationPort();
	///
	std::string &getDestinationIp();

	Semaphore *getTransmitterSempahore();
};

#endif // TRANSMITTER_H