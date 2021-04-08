#ifndef AGENTE_AZUL
#define AGENTE_AZUL
#include "../Green/CommonTypes.h"
#include <vector>
#include <tuple>
#include <string.h> //strcpy, append,c.str
#include "../common/Thread.hpp"

class BlueAgent : public Thread
{
private:
	int writepipe[2];
	int readpipe[2];

	int id;
	int16_t idSource;
	int16_t port;
	std::vector<std::tuple<int, int, std::string>> messageQueue;
	std::vector<std::string> neighborsList;

	void awakenBlueNode();
	void awakenServer();

public:
	BlueAgent();
	~BlueAgent();

	void setPipes();

	void sendNeighbors();
	void receiveNeighborsInfo(std::vector<green_node_identity_t> neighbors_identity_t);
	void getNodeData(green_node_identity_t);
	void sendData();

	void establishConnectionWithNode();
	void establishConnectionWithServer();
	void setGreenId(int);
	bool isYourQueueEmpty();
	void listenToBlueNode();
	void sendToBlueNode(const char *, int);
	std::tuple<int, int, std::string> popMessageFromQueue();
	void fin();
	int getReadEndReadPipe();
	int getWriteEndPipe();
	virtual int run() override;
};

#endif
