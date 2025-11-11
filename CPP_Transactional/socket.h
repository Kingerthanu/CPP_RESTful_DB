#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class Socket
{

private:
	static bool wsaInitialized;

public:
	static void initializeWSA();

	static void cleanupWSA();

	static int createServerSocket(const int& port);

	static int createClientSocket(const std::string& server_ip, const int& server_port);

	static int acceptConnection(const int& server_fd);

	static void closeSocket(const int& socket_fd);

	static int sendData(const int& socket_id, const char* data, const int& length);

	static int receiveData(const int& socket_id, char* buffer, const int& length);


};
