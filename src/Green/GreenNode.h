#ifndef GREEN_NODE_H
#define GREEN_NODE_H

#include "BlueAgent.h"
#include "PinkAgent.h"
#include "BlueConsumer.h"
#include "BlueProducer.h"
#include "PinkProducer.hpp"
#include "PinkConsumer.h"
#include "CommonTypes.h"
#include "fromOtherNodesMiniDispatcher.h"
#include "LinkLayer.h"
#include "./NetworkLayer/NetworkLayer.h"
#include "OrangeAgent.h"
#include "PackagesDispatcher.h"
#include "NeighborsTableDeliver.h"
class GreenNode
{
	/// Class members.
private:

	///Ports for TCP connection 
	const int TCP_PORT_1 = 6000;
	const int TCP_PORT_2 = 6100;

	/// Green node id and port.
	green_node_identity_t greenNodeIdentity;

	/// Green node neighbors identity (id, port, ip).
	neighbors_identity_t neighborsIdentity;

	/// They are the arguments used to execute a specific test case.
	std::vector<std::string> arguments;

	/// The orange agent is the one in charge of creating the orange node process, reading the green node identity and neighors identity.
	OrangeAgent orangeAgent;

	/// The blue-agent is in charge of creating the blue node process and establishing communcation means between the green node and the blue node.
	BlueAgent blueAgent;

	/// The blue-producer is in charge of receiving application messages from the blue node and sending them to the network layer.
	BlueProducer blueProducer;

	/// The blue consumer is in charge of receiving a forwaring payload from the network layer and sending the inner application message to the
	/// blue node.
	BlueConsumer blueConsumer;

	/// The pink agent is the one in charge of creating the pink node process, and puts the pink producer/consumer to work
	PinkAgent* pinkAgent;

	///It is the component that consume request from NetworkLayer and sends the request to pink node
	PinkConsumer* pinkConsumer;

	///It is the component that receive request from pink node and retransmites (produce) results to NetworkLayer
	PinkProducer* pinkProducer;

	/// The network layer is formed by multiple components such as Forwarding and Broadcast. It is the
	/// middle point between Application and Link layers.
	NetworkLayer networkLayer;

	/// The link layer consists of multiple submodules such as transmitters to communicate with other neighbors,
	/// and a listener to receives payloads from other neighbors. Furthermore, it contains the herbeat module.
	LinkLayer linklayer;

	/// This table is a key resource within the Link layer. Basically, it maps every neighbor id with its correspoding transmitter. As a result,
	/// messages are sent to each neighbor.
	std::map<DestinationId, Transmitter *> redirectingTable;

	/// This so called mini-dispatcher is the component in charge of receiving payloads from other nodes and determine if they have
	/// to be redirected to the upper layer or to hearbeat module.
	FromOtherNodesMiniDispatcher miniDispatcher;

	NeighborsTableDeliver neighborsTableDeliver;

	/// Log to record all the events.
	std::ofstream log;

	/// Construction and destruction.
public:
	/// Default constructor.
	GreenNode();

	/// Default destructor.
	~GreenNode();

public: /// Public interface.
	int start(int argc, char *argv[]);

	/// Helpers methods used to initialize and start all the green node's components.
private:
	/// This methods analyzes the command line arguments to determine if all were provided.
	int analyzeArguments(int argc, char *argv[]);

	/// It opens a log file for the green node with a unique identifier taken from the arguments.
	int openLog();

	/// It sends the program arguments to the orange agent that are needed to initialize orange node process execution.
	void sendArgumentsToOrangeAgent();

	/// It requests to orange-agent the information that was read via communication with orange node.
	void obtainDataFromOrangeAgent();

	/// It starts the orange agent execution with the required arguments, and shares to green node the read information.
	void configureAndStartOrangeAgent();

	/// It associates a neighbor's id with a transmitter that will be in charge of sending payloads to the associated neighbor.
	void initializeLinkLayerRedirectingTable();

	/// It sends all the required data (pointers, references, among others) to the link layer.
	void initializeLinkLayer();

	/// It configures and starts the link layer execution.
	void configureAndStartLinkLayer();

	/// It configures and starts the mini-dispatcher its execution.
	void configureAndStartMiniDispatcher();

	/// It configures and starts the blue agent execution.
	void configureAndStartBlueAgent();

	/// It configures and starts the pink agent execution.
	void configureAndStartPinkAgent();

	///It initialize the pink producer and consumer
	void configureComponentsOfPinkAgent();

	/// It configures and starts the blue producer execution.
	void configureAndStartBlueProducer();

	/// It configures and starts the blue consumer execution.
	void configureAndStartBlueConsumer();

	/// It configures and starts the network layer execution.
	void configureAndStartNetworkLayer();

	/// It configures and starts the network layer execution.
	void configureAndStartNeighborsTableDeliver();

	/// It tells the main thread to wait for all the threads that have been spawned.
	void waitForAllThreadsToFinish();

	/// It prints the green node id and port.
	void printGreenNodesIdentity();

	/// It prints the green node's neighbors id, port and ip.
	void printNeighborsIdentity();
};
#endif