#include "GreenNode.h"
#include "Log.hpp"

#include <iostream>
#include <stdlib.h>

const char *const usage =
	"Usage: round case node-id\n"
	"\n"
	"  round    test round\n"
	"  case     test case \n"
	"  node-id  test node \n"
	"\n";

std::string logpath =
	"../src/Green/Files/Logs/green_node_log";

GreenNode ::GreenNode()
{
}

GreenNode ::~GreenNode()
{
	if (pinkProducer != nullptr)
	{
		delete pinkProducer;
		pinkProducer = nullptr;
	}

	if (pinkConsumer != nullptr)
	{
		delete pinkConsumer;
		pinkConsumer = nullptr;
	}
	if (pinkAgent != nullptr)
	{
		delete pinkAgent;
		pinkAgent = nullptr;
	}
}

int GreenNode::start(int argc, char *argv[])
{
	if (int argumentsError = this->analyzeArguments(argc, argv))
	{
		return argumentsError;
	}

	if (int logError = this->openLog())
	{
		return logError;
	}

	this->configureAndStartOrangeAgent();

	this->configureAndStartLinkLayer();

	this->configureAndStartMiniDispatcher();

	this->configureAndStartBlueAgent();

	this->configureAndStartBlueProducer();

	this->configureAndStartBlueConsumer();

	this->configureComponentsOfPinkAgent();

	this->configureAndStartNetworkLayer();

	this->configureAndStartPinkAgent();

	this->configureAndStartNeighborsTableDeliver();

	Log::append("***** Green Node              ***** < All green node components have started. >");

	this->waitForAllThreadsToFinish();

	Log::append("***** Green Node              ***** < All green node components have finished. >");

	this->log.close();

	return EXIT_SUCCESS;
}

int GreenNode::analyzeArguments(int argc, char *argv[])
{
	if (argc != 4)
	{
		std::cout << usage;
		return EXIT_FAILURE;
	}

	for (int i = 0; i < argc; i++)
	{
		this->arguments.push_back(argv[i]);
	}

	return EXIT_SUCCESS;
}

int GreenNode::openLog()
{
	logpath += this->arguments[3] + ".txt";

	this->log.open(logpath);

	if (this->log.fail())
	{
		std::cout << "***** Green Node              ***** < Couldn't open " << logpath << " file. >\n";
		return EXIT_FAILURE;
	}
	else
	{
		Log::append("***** Green Node              ***** < Log file was successfully opened. >");
	}

	// General purpose log ostream is change to the new opened file.
	Log::getInstance().setOutput(this->log);

	return EXIT_SUCCESS;
}

void GreenNode::sendArgumentsToOrangeAgent()
{
	this->orangeAgent.receiveArguments(this->arguments);
}

void GreenNode::obtainDataFromOrangeAgent()
{
	this->greenNodeIdentity = this->orangeAgent.getGreenNodeIdentity();
	this->neighborsIdentity = this->orangeAgent.getNeighborsIdentity();
}

void GreenNode::configureAndStartOrangeAgent()
{
	// Orange agent configuration.
	this->sendArgumentsToOrangeAgent();

	// It starts orange agent execution.
	this->orangeAgent.run();

	// It shares back retrieved data to green node.
	this->obtainDataFromOrangeAgent();
}

void GreenNode::initializeLinkLayerRedirectingTable()
{
	Log::append("***** Green Node              ***** < Fill redirecting and cache table starts. >");
	for (auto &neighbor : this->neighborsIdentity)
	{
		Log::append("***** Green Node              ***** < As key neighbor id: " + std::to_string(neighbor.id) + " As transmiter port and ip (" + std::to_string(neighbor.port) + ", " + neighbor.ip + ") >");
		this->redirectingTable[neighbor.id] = new Transmitter(neighbor.port, neighbor.ip);
	}
}

void GreenNode::initializeLinkLayer()
{
	this->linklayer.receiveNeighborsIdentity(this->neighborsIdentity);
	this->linklayer.receiveRedirectingTable(this->redirectingTable);
	this->linklayer.receiveIdentity(this->greenNodeIdentity);
	Log::append("***** Green Node              ***** < Access to shared data structures was granted to all green node components. >");
}

void GreenNode::configureAndStartLinkLayer()
{
	// Link layer configuration.
	this->initializeLinkLayerRedirectingTable();
	this->initializeLinkLayer();

	// It starts the link layer execution.
	this->linklayer.start();
}

void GreenNode::configureAndStartMiniDispatcher()
{
	// Possible redirecting queues are initialized.
	this->miniDispatcher.setRedirectingQueues(this->linklayer.getHearbeatAccessPoint(), this->networkLayer.getAccessPointForLinkLayer());

	// Link layer listener produces to mini dispatcher queue.
	this->linklayer.setListenerQueue(this->miniDispatcher.getQueue());

	this->miniDispatcher.startThread();
}

void GreenNode::configureAndStartBlueAgent()
{
	// Blue agent configuration.
	this->blueAgent.setPipes();
	this->blueAgent.receiveNeighborsInfo(this->neighborsIdentity);
	this->blueAgent.sendNeighbors();
	this->blueAgent.getNodeData(this->greenNodeIdentity);
	this->blueAgent.sendData();

	// It starts the blue agent execution.
	this->blueAgent.startThread();
}
void GreenNode::configureComponentsOfPinkAgent()
{
	pinkProducer = new PinkProducer();
	pinkProducer->setPortForReceivingFromNode(greenNodeIdentity.id + TCP_PORT_1);
	pinkConsumer = new PinkConsumer();
	pinkConsumer->setPortForSendingToNode(greenNodeIdentity.id + TCP_PORT_2);
	Log::append("***** Green Node              ***** < Pink components are ready. >");
}
void GreenNode::configureAndStartPinkAgent()
{
	pinkProducer->setQueue(networkLayer.getFromPinkAgentDispatcherQueue());
	pinkAgent = new PinkAgent(pinkProducer, pinkConsumer, greenNodeIdentity.id);
	Log::append("***** Green Node              ***** < Pink agent is ready. >");
	pinkAgent->startThread();
	Log::append("***** Green Node              ***** < Pink components are in execution. >");
}

void GreenNode::configureAndStartBlueProducer()
{
	// Blue producer configuration.
	this->blueProducer.receiveFileDescriptor(this->blueAgent.getReadEndReadPipe());
	this->blueProducer.receiveGreenNodeId((int16_t)this->greenNodeIdentity.id);

	// The blue producer produces application payloads for the network layer.
	this->blueProducer.setQueue(this->networkLayer.getAccessPointForApplicationLayer());

	// It starts the blue producer execution.
	this->blueProducer.startThread();
}

void GreenNode::configureAndStartBlueConsumer()
{
	// Blue consumer configuration.
	this->blueConsumer.receiveFileDescriptor(this->blueAgent.getWriteEndPipe());

	// It starts the blue consumer execution.
	this->blueConsumer.startThread();
}

void GreenNode::configureAndStartNetworkLayer()
{
	// Network layer configuration.
	this->networkLayer.setForwardingQueues(this->blueConsumer.getQueue(), this->linklayer.getLinkLayerAcessPoint());
	this->networkLayer.setIdentity(this->greenNodeIdentity.id);
	this->networkLayer.setNeighborsIdentity(this->neighborsIdentity);
	this->networkLayer.setPinkConsumerAcessPoint(this->pinkConsumer->getQueue());
	this->networkLayer.setRedirectingPointsForBroadcaster(this->linklayer.getLinkLayerAcessPoint(), this->pinkConsumer->getQueue());

	// It starts the network layer execution.
	this->networkLayer.start();
}

void GreenNode::configureAndStartNeighborsTableDeliver()
{
	Log::append("***** Green Node              ***** < Configure NeighborsTableDeliver. >");
	this->neighborsTableDeliver.setNeighborsIdentities(this->neighborsIdentity);
	this->neighborsTableDeliver.setBlueConsumerQueue(this->blueConsumer.getQueue());
	this->neighborsTableDeliver.setPinkConsumerQueue(this->pinkConsumer->getQueue());
	this->neighborsTableDeliver.setHeartbeatComponent(this->linklayer.getHearbeatComponent());
	Log::append("***** Green Node              ***** < Start NeighborsTableDeliver. >");
	this->neighborsTableDeliver.startThread();
	Log::append("***** Green Node              ***** < NeighborsTableDeliver has started. >");
}

void GreenNode::waitForAllThreadsToFinish()
{
	//For all transmitters in redirect table transmitter.wait();

	this->linklayer.waitForLinkLayerThreads();
	//blueAgent.wait();
	this->blueProducer.waitToFinish();
	this->blueAgent.waitToFinish();
	this->networkLayer.waitForNetworkComponentsToFinish();
	this->pinkAgent->waitToFinish();
	this->neighborsTableDeliver.terminateWork();
	Log::append("***** Green Node              ***** < Everything has finished. Log file will be closed. >\n");
}

void GreenNode::printGreenNodesIdentity()
{
	std::cout << "***** Green Node              ***** < The green node identity is ID: " << greenNodeIdentity.id << " Port: " << greenNodeIdentity.port << " >\n"
			  << std::endl;
}

void GreenNode::printNeighborsIdentity()
{
	std::cout << "***** Green Node              ***** < The neighbors identity is as follows: \n"
			  << std::endl;
	for (auto &t : neighborsIdentity)
	{
		std::cout << "Neighbor information" << std::endl;
		std::cout << "Id is: " << t.id << std::endl
				  << "Ip is: " << t.ip << std::endl
				  << "Port is: " << t.port << " >\n"
				  << std::endl;
	}
}