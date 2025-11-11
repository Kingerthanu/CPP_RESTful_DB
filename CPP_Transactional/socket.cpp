#include "socket.h"


bool Socket::wsaInitialized = false;

int Socket::receiveData(const int& socket_id, char* buffer, const int& length)
{

	int totalReceived = 0;
	while (totalReceived < length)
	{
		int received = recv(socket_id, buffer + totalReceived, length - totalReceived, 0);
		if (received == SOCKET_ERROR)
		{
			throw std::runtime_error("Receive failed with error: " + std::to_string(WSAGetLastError()));
		}
		else if (received == 0)
		{
			break; // Connection closed
		}
		totalReceived += received;
	}

	return totalReceived;

};

void Socket::initializeWSA()
{

	if (!wsaInitialized)
	{
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (result != 0)
		{
			throw std::runtime_error("WSAStartup failed with error: " + std::to_string(result));
		}

		wsaInitialized = true;
	}

};

void Socket::cleanupWSA()
{

	if (wsaInitialized)
	{
		WSACleanup();
		wsaInitialized = false;
	}

};

int Socket::createServerSocket(const int& port)
{

	// Initialize WSA If Not Already Done
	initializeWSA();

	std::cout << "Creating server socket on port " << port << std::endl;

	// Create Socket
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == INVALID_SOCKET)
	{
		throw std::runtime_error("Socket creation failed with error: " + std::to_string(WSAGetLastError()));
	}

	std::cout << "Socket created successfully." << std::endl;

	// Set Socket Options
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) == SOCKET_ERROR)
	{
		closesocket(server_fd);
		throw std::runtime_error("Set socket options failed with error: " + std::to_string(WSAGetLastError()));
	}

	std::cout << "Socket options set successfully." << std::endl;

	// Setup Server Address Structure
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	std::cout << "Server address structure set up." << std::endl;
	
	// Replace line 58 with:
	char ip_str[INET_ADDRSTRLEN];
	if (address.sin_addr.s_addr == INADDR_ANY) {
		std::cout << "Server bound to ALL interfaces (0.0.0.0)" << std::endl;
		std::cout << "Accessible via localhost, 127.0.0.1, or any local IP" << std::endl;
	}
	else {
		inet_ntop(AF_INET, &address.sin_addr, ip_str, INET_ADDRSTRLEN);
		std::cout << "Server bound to IP: " << ip_str << std::endl;
	}
	std::cout << "Port: " << port << std::endl;

	// Bind Socket
	if (bind(server_fd, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		closesocket(server_fd);
		throw std::runtime_error("Bind failed with error: " + std::to_string(WSAGetLastError()));
	}

	std::cout << "Socket bound successfully." << std::endl;
	std::cout << "Server is listening on port " << port << std::endl;
	

	// Start Listening
	if (listen(server_fd, 5) == SOCKET_ERROR)
	{
		closesocket(server_fd);
		throw std::runtime_error("Listen failed with error: " + std::to_string(WSAGetLastError()));
	}

	std::cout << "Socket is now listening for incoming connections." << std::endl;


	return server_fd;

}

int Socket::createClientSocket(const std::string& server_ip, const int& server_port)
{

	// Initialize WSA If Not Already Done
	initializeWSA();

	std::cout << "Creating client socket to " << server_ip << ":" << server_port << std::endl;

	// Create Socket
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == INVALID_SOCKET)
	{
		throw std::runtime_error("Socket creation failed with error: " + std::to_string(WSAGetLastError()));
	}

	std::cout << "Socket created successfully." << std::endl;

	// Setup Server Address Structure
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);

	std::cout << "Server address structure set up." << std::endl;

	// Convert IP Address
	if (inet_pton(AF_INET, server_ip.c_str(), &server_address.sin_addr) <= 0)
	{
		closesocket(socket_fd);
		throw std::runtime_error("Invalid address/ Address not supported");
	}

	std::cout << "IP address converted successfully." << std::endl;

	// Connect To Server
	if (connect(socket_fd, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR)
	{
		closesocket(socket_fd);
		throw std::runtime_error("Connection to server failed with error: " + std::to_string(WSAGetLastError()));
	}

	std::cout << "Connected to server " << server_ip << ":" << server_port << std::endl;


	return socket_fd;

};

int Socket::acceptConnection(const int& server_fd)
{

	// Create Client Address Structure
	sockaddr_in client_address;
	int addrlen = sizeof(client_address);

	// Accept Connection
	int client_fd = accept(server_fd, (sockaddr*)&client_address, &addrlen);
	if (client_fd == INVALID_SOCKET)
	{
		throw std::runtime_error("Accept failed with error: " + std::to_string(WSAGetLastError()));
	}


	return client_fd;

};

void Socket::closeSocket(const int& socket_fd)
{

	if (socket_fd != INVALID_SOCKET)
	{
		closesocket(socket_fd);
	}

};

int Socket::sendData(const int& socket_id, const char* data, const int& length)
{

	int totalSent = 0;
	while (totalSent < length)
	{
		int sent = send(socket_id, data + totalSent, length - totalSent, 0);
		if (sent == SOCKET_ERROR)
		{
			throw std::runtime_error("Send failed with error: " + std::to_string(WSAGetLastError()));
		}
		totalSent += sent;
	}


	return totalSent;

};
