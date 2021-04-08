#include "PinkAgent.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h> // wait
#include <sys/wait.h>  //wait

PinkAgent::PinkAgent(PinkProducer *pinkProducer, PinkConsumer *pinkConsumer, int16_t &greenNodeId)
{
	this->pinkConsumer = pinkConsumer;
	this->pinkProducer = pinkProducer;
	this->greenNodeId = greenNodeId;
}

PinkAgent::~PinkAgent()
{
	if (pinkConsumer != nullptr)
	{
		delete pinkConsumer;
	}

	if (pinkProducer != nullptr)
	{
		delete pinkProducer;
	}

	pinkProducer = nullptr;
	pinkConsumer = nullptr;
}

int PinkAgent::run()
{
	this->work();
	return EXIT_FAILURE;
}

void PinkAgent::work()
{
	pid_t processId = fork();
	if (processId == 0)
	{

		//It is a child process
		Log::append("***** Pink Agent              ***** < Before execlp. >");
		execlp(
			"python3", "python3", "../src/PinkNode/main.py",
			std::to_string(pinkConsumer->getPortForSendingToNode()).c_str(),
			std::to_string(pinkProducer->getPortForReceivingToNode()).c_str(),
			std::to_string(this->greenNodeId).c_str(),
			(char *)0);
		Log::append("***** Pink Agent              ***** < Error in execlp. >");
	}
	else
	{
		//It is a father
		pinkProducer->startThread();
		pinkConsumer->startThread();
		Log::append("***** Pink Agent              ***** < Consumer/Producer has started. >");
		pinkConsumer->waitToFinish();
		pinkProducer->waitToFinish();
		Log::append("***** Pink Agent              ***** < Consumer/Producer has finished, waiting for child process >");
		wait(NULL); //wait for his child
		Log::append("***** Pink Agent              ***** < Child process has finished, then pink agent can finish >");
	}
}