#ifndef FROM_OTHER_NODES_MINI_DISPATCHER_H
#define FROM_OTHER_NODES_MINI_DISPATCHER_H

#include "../common/Consumer.hpp"
#include "../common/Payload.hpp"
#include "../common/Queue.hpp"

class FromOtherNodesMiniDispatcher : public Consumer<Payload::payload_ptr>
{
	/// Constants.
public:
	static const uint8_t NETWORK_LAYER_QUEUE = 1;
	static const uint8_t HEARTBEAT_COMPONENT_QUEUE = 2;

	/// Class members.
private:
	// It is used in case that the received payload is hearbeat type.
	Queue<Payload::payload_ptr> *toHearbeatComponentAccessPoint;

	// It is used in case that the received payload is network type.
	Queue<Payload::payload_ptr> *toNetworkLayerAccessPoint;

public:
	/// Default constructor.
	FromOtherNodesMiniDispatcher();

	/// Default destructor.
	~FromOtherNodesMiniDispatcher();

public:
	// It initializes inner access points.
	void setRedirectingQueues(Queue<Payload::payload_ptr> *toHearbeatComponentAccessPoint,
							  Queue<Payload::payload_ptr> *toNetworkLayerAccessPoint);

	/// Inherited methods.
protected:
	virtual void consume(const Payload::payload_ptr &payload);
	virtual int run();
};

#endif // FROM_OTHER_NODES_MINI_DISPATCHER_H