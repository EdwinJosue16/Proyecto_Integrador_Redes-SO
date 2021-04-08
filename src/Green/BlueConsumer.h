#ifndef BLUE_CONSUMER
#define BLUE_CONSUMER
#include <unistd.h> //pipe
#include "../common/Consumer.hpp"
#include "../common/Thread.hpp"
#include "CommonTypes.h"
#include "Log.hpp"
#include "../common/Queue.hpp"

class BlueConsumer : public Consumer<payload_ptr>
{
	DISABLE_COPY(BlueConsumer);

	int fileDescriptorWriteEndPipe;

public:
	/// Default constructor.
	BlueConsumer();

	/// Default destructor.
	~BlueConsumer();

	/// Consume the messages in its own execution thread
	virtual int run() override;

	/// Override this method to process any data extracted from the queue
	virtual void consume(const payload_ptr &package) override;

	inline void receiveFileDescriptor(int fileDescriptorWriteEndPipe)
	{
		this->fileDescriptorWriteEndPipe = fileDescriptorWriteEndPipe;
	}
};
#endif
