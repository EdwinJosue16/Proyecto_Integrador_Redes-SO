#include "TcpSocketWrapper.h"

TcpSocketWrapper::TcpSocketWrapper()
{
	this->port=12345; //DEFAULT PORT
	this->IP="127.0.0.1"; //localhost is defult "IP"
}

TcpSocketWrapper::TcpSocketWrapper(int port)
{
	this->port = port;
	this->IP="127.0.0.1"; //localhost is defult "IP"
}

TcpSocketWrapper::TcpSocketWrapper(int port, std::string)
{ 
    this->port=port;
	this->IP=IP;
}

TcpSocketWrapper::~TcpSocketWrapper()
{ 
    close(fdSocketForBiding);
}


void TcpSocketWrapper::build()
{
	bindConnection();
	acceptConnection();
}

void TcpSocketWrapper::bindConnection()
{
	Log::append("***** TCP_Socket_Wrapper          ***** < Binding process has started, the port number is "+std::to_string(port) +" >");
	if ((fdSocketForBiding = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
			Log::append("***** TCP_Socket_Wrapper          ***** <  Socket's file descriptor's creation failed. >"); 
			exit(EXIT_FAILURE); 
	} 
	
	socketAddress.sin_family = AF_INET; 
    socketAddress.sin_addr.s_addr = htonl(INADDR_ANY); 
    socketAddress.sin_port = htons(port); 
	
	if (bind(fdSocketForBiding, (struct sockaddr *)&socketAddress,sizeof(socketAddress))<0) 
    { 
        Log::append("***** TCP_Socket_Wrapper          ***** < Binding process failed. >");
        exit(EXIT_FAILURE); 
    } 
	Log::append("***** TCP_Socket_Wrapper          ***** < Binding  process has finished >");
}

void TcpSocketWrapper::acceptConnection()
{
	Log::append("***** TCP_Socket_Wrapper          ***** < Accept connection  process has started >");
	listen(fdSocketForBiding,NUMBER_OF_CONNECTIONS);
	sockaddr_in tcpAddr;
	socklen_t tcp_size = sizeof(socketAddress); //sizeof(struct sockaddr_in);
	
	if ((tcpSocket = accept(fdSocketForBiding, (struct sockaddr *)&tcpAddr, &tcp_size)) < 0)
	{
		std::cout << "Accept process failed." << std::endl;
		exit(EXIT_FAILURE);
	}
	Log::append("***** TCP_Socket_Wrapper          ***** < Accept connection  has finished >");

}