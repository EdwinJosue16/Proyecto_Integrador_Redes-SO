#ifndef TCP_SOCKET_WRAPPER
#define TCP_SOCKET_WRAPPER


#include <unistd.h> //close(fd)
#include <sys/socket.h> //socket
#include <arpa/inet.h> // struct sockaddr_in
#include <unistd.h> // read, write functions
#include <string>
#include "Log.hpp"

class TcpSocketWrapper
{
private:

    ///Actually is not necessary (using localhost)
    struct hostent * server;
    ///
    const int NUMBER_OF_CONNECTIONS = 1;

	///
	struct sockaddr_in socketAddress;

    ///
    sockaddr_in tcpAddrInfo;

	/// This is the final tcp connection socket
	int tcpSocket;

	/// This is the file descriptor, it is used for building the socket
	int fdSocketForBiding;

	/// In this port for socket
	int port;
    
    /// This de IP for socket
    std::string IP;

private:

    ///
    void bindConnection();

	///
	void acceptConnection();



public:

    ///
    TcpSocketWrapper();
     
    ///
     TcpSocketWrapper(int port);     
    ///
     TcpSocketWrapper(int port, std::string IP);
    /// 
    ~TcpSocketWrapper();

	///This method builds the socket
	void build();

    ///
    inline int getFileDescriptor()
    {
        return this->tcpSocket;
    }
};
#endif