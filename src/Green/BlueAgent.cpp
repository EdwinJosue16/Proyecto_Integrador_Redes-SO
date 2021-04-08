#include "BlueAgent.h"

#include <iostream>	   //cout,endl
#include <unistd.h>	   //exclp,pipe, getcwd, read, write
#include <sys/types.h> // wait
#include <sys/wait.h>  //wait
#include <sys/ipc.h>
#include "../Green/Log.hpp"

#define RE_WRITEPIPE writepipe[0]
#define WE_WRITEPIPE writepipe[1]

// Readpipe.
#define RE_READPIPE readpipe[0]
#define WE_READPIPE readpipe[1]

// Setup of readpipe.

#define WE_READPIPE_FD_MSS "RPWEFD"
#define READPIPE_FDES_SIZE 15

#define RPWE_RECEPTION_MESSAGE_SIZE 6
#define FD_RECEIVED "FDRCV"

// End communication.
#define END_COMMUNICATION "END"
#define END_COMMUNICATION_SIZE 3

#define ALL_NEIGHGBOURS_SENT "ALL"

#define SOURCE_ID 1

// Other.
#define OUTPUT_PIPE_MESSAGE_SIZE 50
#define INPUT_PIPE_MESSAGE_SIZE 256

const int WRITEPIPE_CREATION_ERROR = 1;
const int READPIPE_CREATION_ERROR = 2;

/**.
\brief  Constructor method for blue agent.
\details This method is used to create instances of the blue agent class. 
\param[in] value id:integer number that represents the node's identification 
\param[out] buffer rPipe: int array that the pipe() function will use to store the readpipe's fd values
\param[out] buffer wPipe:int array that the pipe() function will use to store the writepipe's fd values
\pre The invoker must declare 2 int arrays and have acces to the node's id so it can use them to create a blue agent's instance 
\remark wPipe and rPipe will contain the pipes' fd values once this method is done,
\return no value is returned
\exception none As long as the correct value types are introduced as parameters, no exception should be thrown
\author Eddy Ruiz Soto 
\date September 3rd,2020-October 3rd 2020
*/

BlueAgent::BlueAgent()
{
}

void BlueAgent::setPipes()
{
	if ((pipe(readpipe) < 0) || (pipe(writepipe) < 0))
	{
		std::cout << "An error ocurred while creating blue pipes.";
		exit(0);
	}
}

void BlueAgent::setGreenId(int id)
{
	this->id = id;
}

void BlueAgent::receiveNeighborsInfo(neighbors_identity_t vector)
{
	std::vector<green_node_identity_t>::iterator actIterator;

	for (actIterator = vector.begin(); actIterator != vector.end(); actIterator++)
	{
		neighborsList.push_back(std::to_string((*actIterator).id));
	}
}

void BlueAgent::sendNeighbors()
{

	while (neighborsList.empty() != 1)
	{
		std::string neighbor = neighborsList.front();
		sendToBlueNode(neighbor.c_str(), 5);
		neighborsList.erase(neighborsList.begin());
	}
	sendToBlueNode(ALL_NEIGHGBOURS_SENT, 5);
}

void BlueAgent::getNodeData(green_node_identity_t sourceData)
{
	this->idSource = sourceData.id;
	this->port = sourceData.port;
	std::cout<< "El id es "<< idSource << " y el puerto es " << port <<std::endl;
}

void BlueAgent::sendData()
{
	char vector[3] = {0};
	memcpy(vector, &this->idSource , 2);
	sendToBlueNode(vector, 2);
	memcpy(vector, &this->port , 2);
	sendToBlueNode(vector , 2);
	
}


/**
\brief Method used to set up communications with the blue node.
\details  First, a fork() method is created. The child process will manage the agent's side of the set up process. Meanwhile, the parent will wake the node up.
			Second, the stdin's value is replaced by one of the pipes' fd so agent and node can communicate over that stream
			Third, blue agent sends over stdin the pipes' fd values the blue node needs to know so they can comunicate without stdin.
			Finally, blue agent sends the stdin's original value to blue node, and sets stdin's value back to its original number.
\param No input parameter is required 
\pre The invoker needs to make sure that the agent's writepipe and readpipe values are initialized.
\remark Stdin's fd value gets temporarily modified 
\return 0, as way to confirm that the set up went ok-
\exception The only exception that could happen is if the fork() function fails, which hardly ever happens.
\author Eddy Ruiz, based on Dennis Solano's code.
\date October 1st, 2020
*/

void BlueAgent::establishConnectionWithNode()
{
	if (fork() != 0)
	{
		Log::append("***** Blue Agent              ***** < Wait for the son >");
		wait(NULL);
		Log::append("***** Blue Agent              ***** < Blue node execution has finished. >");
	}
	else
	{
		Log::append("***** Blue Agent              ***** < Blue node execution has started. >");
		awakenBlueNode();
		Log::append("***** Blue Agent              ***** < Blue node execution has failed. >");
	}
}

/**
\brief This method is used to run the python script that allows the blue node's creation
\details dup2 function is used to momentarily replace stdin's value for a pipe, so blue node can listen to its agent over that stream.
\param No value is required as parameter. 
\pre Agent's writepipe and readpipe attributes must be initialized by the time this method is invoked. 
							 Also, agent's oldStdin value must contain a copy of stdin's original value.
\return No return value.
\exception An exception might be thrown if the blue node's file is not found in the same folder as the agent's .cc file.
\author Eddy Ruiz
\date October 2nd 
*/

void BlueAgent::awakenBlueNode()
{
	execlp("python3", "python3", "../src/Blue/websocket.py",  std::to_string(WE_READPIPE).c_str(),
		   std::to_string(RE_WRITEPIPE).c_str(), (char *)0);
	Log::append("***** Blue Agent              ***** < Error: Blue node execution has finished. >");
}

/**
\brief Destructor method. Invoked when blue agent will end its operationr.
\details Closes all pipes' fd values 
\param No parameters is required
\pre Agent's writepipe and readpipe must be initialized
\remark Pipes get closed, so communication with blue node is ended
\return No return value.
\exception The only way this method can throw an exception is if the pipes were not initialized in the first place.
\author Eddy Ruiz
\date September 3rd 2020, October 3rd
*/
BlueAgent::~BlueAgent()
{
	close(RE_WRITEPIPE);
	close(WE_WRITEPIPE);

	close(RE_READPIPE);
	close(WE_READPIPE);
}

/**
\brief Method used to send data to blue node.
\details A char array, as big as the parameter integer size indicates, is initialized and filed with null characters
		    Then, it is sent to blue node through the writepipe's writing end.
\param[in] buffer value inp, a car array that contains the message to send to blue node
\param[in] value size, an integer that indicates the size of inp.
\pre Agent's pipes must be initalized. Also, blue node must have received its pipes at this point.
\return No value is returned 
\exception No exception could be thrown
\author Eddy Ruiz
\date September 5th,2020- October 5th,2020.
*/

//recibe in, char* de mensaje para nodo
void BlueAgent::sendToBlueNode(const char *inp, int size)
{
	int sz = size;

	if (size < 0)
	{
		sz = 100;
	}

	char input[sz];
	memset(input, '\0', sz);

	strcat(input, inp);
	write(WE_WRITEPIPE, input, sz);
}

/**
\brief Sends ending mesage to blue node
\details This method sends the END_COMMUNICATION keyword to blue node, to let it know that it must cease operations
\param No parameter required. 
\pre Agent's pipes must be initalized
\return  No return value.
\exception No exception should be thrown.
\author Eddy Ruiz
\date October 3rd, 2020
*/

void BlueAgent::fin()
{
	sendToBlueNode((char *)END_COMMUNICATION, END_COMMUNICATION_SIZE);
}

bool BlueAgent::isYourQueueEmpty()
{
	return messageQueue.empty();
}

/**
\brief Method used to retrieve messages from blue agent's messageQueue.
\details This method copies the first std tuple that is found in the messageQueue. 
			It keeps the copy so it can return it and makes the queue erase the original message.  
\param No parameter is required.
\pre  messageQueue needs to have messages in the first place.
\remark  messageQueue loses one mesage for each time this method is invoked
\return a copy of messageQueue's front();
\exception No exceptions should be thrown.
\author Eddy Ruiz
\date October 3rd,2020 
*/

std::tuple<int, int, std::string> BlueAgent::popMessageFromQueue()
{ /**
	\brief  messageQueue is thr vector where messages from user are stored. It receives tuples<int,int,string>
	where each space of the tuple represents a specific part of the payload sent from blue node's user.
	*/
	std::tuple<int, int, std::string> messageToDispatch = messageQueue.front();
	messageQueue.erase(messageQueue.begin());

	return messageToDispatch;
}

int BlueAgent::getReadEndReadPipe()
{
	return RE_READPIPE;
}

int BlueAgent::getWriteEndPipe()
{
	return WE_WRITEPIPE;
}

int BlueAgent::run()
{
	this->establishConnectionWithNode();
	return EXIT_SUCCESS;
}
