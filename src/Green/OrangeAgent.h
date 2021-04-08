#ifndef ORANGE_AGENT_H
#define ORANGE_AGENT_H

#include "CommonTypes.h"
#include <string>
#include <fstream>

class OrangeAgent
{
private: // Class members.
	int writepipe[2];
	int readpipe[2];
	green_node_identity_t greenNodeIdentity;
	neighbors_identity_t neighborsIdentity;
	//std::ofstream log;
	std::vector<std::string> arguments;

public: // Construction and destructions.
	OrangeAgent();
	~OrangeAgent();

public: // Helpers.
	void run();
	green_node_identity_t &getGreenNodeIdentity();
	neighbors_identity_t &getNeighborsIdentity();
	void receiveArguments(std::vector<std::string> &arguments);

private:
	void sendMessageToOrangeNode(std::string message);
	void receiveMessageFromOrangeNode(std::string &readData);
	int createPipes();
	void setUpPipesForParent();
	void setUpPipeForChild();
	green_node_identity_t tokenizeNodeIdentity(std::string &identity);
	void saveGreenNodeIdentity(std::string &nodeIdentity);
};

#endif