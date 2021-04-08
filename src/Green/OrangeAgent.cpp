#include "OrangeAgent.h"

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h> // for wait()
#include <unistd.h>	  // for fork()

#include "Log.hpp"

// Writepipe.
#define RE_WRITEPIPE writepipe[0]
#define WE_WRITEPIPE writepipe[1]

// Readpipe.
#define RE_READPIPE readpipe[0]
#define WE_READPIPE readpipe[1]

// Errors.
#define WRITEPIPE_CREATION_ERROR 1;
#define READPIPE_CREATION_ERROR 2;

// Messages.

#define READ_IDENTITY_FILE 1
#define READ_NEIGHBORS_FILE 2
#define SEND_NEIGHBOR_IDENTITY 3
#define END_CONNECTION 4
#define CRITICAL_ERROR 5
#define SEND_NEIGHBORS_COUNT 6
#define MSSG_SIZE 1

#define READ_BUFFER_SIZE 200

OrangeAgent::OrangeAgent()
{
	//log.open("./Files/Logs/OrangeAgentLog.txt");
}

OrangeAgent::~OrangeAgent()
{
	close(WE_WRITEPIPE);
	close(RE_WRITEPIPE);
	close(WE_READPIPE);
	close(RE_READPIPE);
}

void OrangeAgent::run()
{
	createPipes();
	pid_t pid = fork();

	if (pid != 0)
	{ // Parent process.

		Log::append("***** Orange Agent            ***** < The parent process has started. >");

		// Pipes are setup for parent process.
		setUpPipesForParent();

		// It requests the orange node to read the green node identity file.
		sendMessageToOrangeNode(std::to_string(READ_IDENTITY_FILE));

		Log::append("***** Orange Agent            ***** < A message was sent to the orange node requesting the green node identity. >");

		// It receives the green node identity from the orange node.
		std::string nodeIdentity;
		receiveMessageFromOrangeNode(nodeIdentity);
		saveGreenNodeIdentity(nodeIdentity);

		Log::append("***** Orange Agent            ***** < The green node identity is: (" + std::to_string(this->greenNodeIdentity.id) + ", " + std::to_string(this->greenNodeIdentity.id) + ") >");

		// It requests the orange node to read the neighbors identity file.
		sendMessageToOrangeNode(std::to_string(READ_NEIGHBORS_FILE));
		Log::append("***** Orange Agent            ***** < A message was sent to the orange node requesting the neighbors file reading. >");

		// It requests the total number of neighbors.
		sendMessageToOrangeNode(std::to_string(SEND_NEIGHBORS_COUNT));
		Log::append("***** Orange Agent            ***** < A message was sent to the orange node requesting the total amount of neighbors. >");

		// It receives the total number of neighbors.
		std::string numberOfNeighbors;
		receiveMessageFromOrangeNode(numberOfNeighbors);
		Log::append("***** Orange Agent            ***** < The number of neighbors nodes is: " + numberOfNeighbors + " >");

		int neighborsCount = std::stoi(numberOfNeighbors.c_str());
		//It request as many neighbors as neighbors count says.
		for (int i = 0; i < neighborsCount; i++)
		{
			Log::append("***** Orange Agent            ***** < A message was sent to the orange node requesting the information of the ith neighbor node. >");

			// It requests the i-th neighbor identity.
			sendMessageToOrangeNode(std::to_string(SEND_NEIGHBOR_IDENTITY));

			// It receives the i-th neighbor identity.
			std::string neighborIdentity;
			receiveMessageFromOrangeNode(neighborIdentity);
			neighborsIdentity.push_back(tokenizeNodeIdentity(neighborIdentity));
		}

		for (auto &element : neighborsIdentity)
		{
			Log::append("***** Orange Agent            ***** < Neighbor identity: Id: " + std::to_string(element.id) + " Ip: " + element.ip + " Port: " + std::to_string(element.port) + " >");
		}

		sendMessageToOrangeNode(std::to_string(END_CONNECTION));

		wait(NULL);

		Log::append("***** Orange Agent            ***** < Orange agent has finished. >");
	} //
	else
	{ // Child process.
		setUpPipeForChild();
		Log::append("***** Orange Agent            ***** < The orange node execution will begin; its process id is " + std::to_string(getpid()) + " >");
		execlp("python3", "python3", "../src/Orange/OrangeNode.py", std::to_string(WE_READPIPE).c_str(), std::to_string(RE_WRITEPIPE).c_str(), arguments[1].c_str(), arguments[2].c_str(), arguments[3].c_str(), (char *)NULL);
	}
}

void OrangeAgent::setUpPipesForParent()
{
	// The parent process only writes messages on the writepipe. It never receives messages.
	close(RE_WRITEPIPE);

	// The parent process only reads messages on the readpipe. It never writes messages.
	close(WE_READPIPE);
}

void OrangeAgent::setUpPipeForChild()
{
	// The child process only receives messages from the writepipe. It never writes messages.
	close(WE_WRITEPIPE);

	// The child process only sends messages trough the readpipe(parent's perspective). It never read messages (The one who reads is the parent).
	close(RE_READPIPE);
}

void OrangeAgent::sendMessageToOrangeNode(std::string message)
{
	write(WE_WRITEPIPE, message.c_str(), MSSG_SIZE);
}

void OrangeAgent::receiveMessageFromOrangeNode(std::string &readData)
{
	char buffer[READ_BUFFER_SIZE];
	read(RE_READPIPE, buffer, READ_BUFFER_SIZE);
	readData = std::string(buffer);
}

green_node_identity_t &OrangeAgent::getGreenNodeIdentity()
{
	return greenNodeIdentity;
}

neighbors_identity_t &OrangeAgent::getNeighborsIdentity()
{
	return neighborsIdentity;
}

int OrangeAgent::createPipes()
{
	if (pipe(writepipe) == -1)
	{
		std::cout << "***** Orange Agent            ***** < Unable to create writepipe. >\n"
				  << std::endl;
		return WRITEPIPE_CREATION_ERROR;
	}

	if (pipe(readpipe) == -1)
	{
		std::cout << "***** Orange Agent            ***** < Unable to create readpipe. >\n"
				  << std::endl;
		return READPIPE_CREATION_ERROR;
	}
	return 0;
}

void OrangeAgent::receiveArguments(std::vector<std::string> &arguments)
{
	this->arguments = arguments;
}

green_node_identity_t OrangeAgent::tokenizeNodeIdentity(std::string &identity)
{
	std::stringstream strintToTokenize(identity);
	std::vector<std::string> tokens;
	std::string intermediate;

	while (getline(strintToTokenize, intermediate, ','))
	{
		tokens.push_back(intermediate);
	}

	return {(int16_t)std::stoi(tokens[0]), tokens[1], (int16_t)std::stoi(tokens[2])};
}

void OrangeAgent::saveGreenNodeIdentity(std::string &nodeIdentity)
{
	size_t commaPos = nodeIdentity.find_first_of(',');

	std::string nodeId = nodeIdentity.substr(0, commaPos);
	std::string nodePort = nodeIdentity.substr(commaPos + 1, nodeIdentity.length());

	greenNodeIdentity.id = std::stoi(nodeId);
	greenNodeIdentity.ip = "";
	greenNodeIdentity.port = std::stoi(nodePort);
}