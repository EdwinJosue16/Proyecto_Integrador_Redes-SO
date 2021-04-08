#ifndef FORWARDING_UPDATE_RECEIVER_H
#define FORWARDING_UPDATE_RECEIVER_H

#include "../../common/Consumer.hpp"
#include "../../common/Payload.hpp"
#include "ForwardingSendingSide.h"
#include "ForwardingReceivingSide.h"

class ForwardingUpdateReceiver : public Consumer<Payload::payload_ptr>
{
private:
	/// When an forwarding table update is received both modules are notified.
	ForwardingSendingSide *forwardingSendingSide;
	ForwardingReceivingSide *forwardingReceivingSide;

public:
	/// Default constructor.
	ForwardingUpdateReceiver();

	/// Default destructor.
	~ForwardingUpdateReceiver();

	/// Inherited must implement methods.
public:
	/// It starts thread execution.
	int virtual run() override;

	/// Override this method to process any data extracted from the queue
	virtual void consume(const Payload::payload_ptr &package) override;

public:
	void setForwardingComponents(ForwardingSendingSide *forwardingSendingSide, ForwardingReceivingSide *forwardingReceivingSide);
};

#endif // FORWARDING_UPDATE_RECEIVER_H