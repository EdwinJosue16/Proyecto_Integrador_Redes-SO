#ifndef TRANSMITERS_CONTROLLER_H
#define TRANSMITERS_CONTROLLER_H

#include "../common/Consumer.hpp"
#include "../common/Semaphore.hpp"
#include "CommonTypes.h"
#include "Transmitter.h"
#include "Heartbeat.hpp"
class TransmittersController : public Consumer<payload_ptr>
{
private:
	/// Green node identity.
	green_node_identity_t greenNodeIdentity;

	/// Total amount of neighbors.
	int neighborsCount;

	/// It is a redirection table used to redirect a package with a given id to a specific transmitter.
	std::map<DestinationId, Transmitter *> redirectingTable;

	Semaphore *transmitterReady;

	/// This is for know the neighbour status before send Payload
	Heartbeat* heartbeat;

public:
	/// Constructor.
	TransmittersController();

	/// Destructor.
	~TransmittersController();

public: /// Public interface.
	/// Consume the messages in its own execution thread
	virtual int run() override;

	/// Override this method to process any data extracted from the queue
	virtual void consume(const payload_ptr &package) override;

	///
	void receiveGreenNodeIdentity(green_node_identity_t &greenNodeIdentity);

	///
	void receiveRedirectingTable(std::map<DestinationId, Transmitter *> &redirectingTable);

	/// 
	void receiveHeartbeatComponent(Heartbeat* heartbeat);

	Semaphore *getTransmittersReadySempahore();

private: /// Utilities.
	///
	payload_ptr preparePackage(payload_ptr package);

	///It takes a decision over the payload (based on neighbour status)
	void sendPayloadToTranssmiter(std::shared_ptr<LinkLayerPayload> payload);

	///
	int32_t ipToInt32(std::string ip);

	///
	void startTranmitters();
};

#endif // LINK_LAYER_INPUT_CONSUMER_H