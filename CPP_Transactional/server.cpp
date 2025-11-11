#include "server.h"
#include "socket.h"
#include <iostream>
#include <thread>
#include <sstream>


Server::Server(const int& port)
{

	// Create Server Socket On Given Port
	this->server_fd = Socket::createServerSocket(port);
	this->running = false;

	// Store Port For Reference
	this->port_fd = port;

};

Server::~Server()
{
	stop();
}

void Server::saveDatabase(const std::string& fileName)
{
	broker.saveToFile(fileName);
};

void Server::start()
{

	this->running = true;

	// Listen For Incoming Connections
	while (this->running)
	{

		try
		{
			int client_fd = Socket::acceptConnection(this->server_fd);
			std::thread(&Server::handleClient, this, client_fd).detach();
			
			// Wait A Bit Before Accepting Next Connection
			std::this_thread::sleep_for(std::chrono::seconds(1));

		}
		catch (const std::exception& e)
		{
			if (this->running)
			{
				std::cerr << "Error accepting connection: " << e.what() << std::endl;
			}
		}

	}

};

void Server::sendLine(int client_fd, const std::string& line)
{

	std::string data = line + "\n";
	Socket::sendData(client_fd, data.c_str(), static_cast<int>(data.size()));

};

void Server::readLine(int client_fd, std::string& line)
{

	line.clear();
	char buffer[1];
	while (true)
	{

		int bytes_received = Socket::receiveData(client_fd, buffer, 1);
		if (bytes_received <= 0)
		{
			throw std::runtime_error("Connection closed or error occurred while reading data.");
		}
		if (buffer[0] == '\n')
		{
			break;
		}
		line += buffer[0];

	}

};

void Server::stop()
{
	this->running = false;
	Socket::closeSocket(this->server_fd);
};

void Server::handleClient(const int client_fd)
{

	try
	{
		std::string line;
		int clientID = client_fd; // Using client_fd as a simple client identifier
		
		while (true)
		{

			// Read Command From Client
			readLine(client_fd, line);

			// Process Command (If Empty, Then Retry)
			if (line.empty()) continue;

			// Parse Command
			std::istringstream iss(line);
			std::string command;
			iss >> command;

			// Handle Commands
			if (command == "GET")
			{
				// Extract Key
				std::string key;
				iss >> key;

				// Get Value From Broker And Send Back To Client
				std::string value = broker.get(key, clientID);
				sendLine(client_fd, value);
			}
			else if (command == "PUT")
			{
				// Extract Key And Value
				std::string key, value;
				iss >> key >> value;

				// Put Key-Value Pair In Broker
				broker.put(key, value, clientID);
				sendLine(client_fd, "OK");
			}
			else if (command == "BEGIN")
			{
				// Begin Transaction For This Client
				broker.beginTransaction(clientID);
				sendLine(client_fd, "OK");
			}
			else if (command == "ABORT")
			{
				// Abort Transaction For This Client
				broker.abortTransaction(clientID);
				sendLine(client_fd, "OK");
			}
			else if (command == "COMMIT")
			{
				// Commit Transaction For This Client
				broker.commitTransaction(clientID);
				sendLine(client_fd, "OK");
			}
			else if (command == "EXIT")
			{
				// Close Connection
				break;
			}
			else
			{
				// Unknown Command
				sendLine(client_fd, "ERROR: Unknown command");
			}

			// Small Delay To Prevent CPU Overuse
			std::this_thread::sleep_for(std::chrono::milliseconds(60));

		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error handling client: " << e.what() << std::endl;
	}


	// Close Client Socket
	Socket::closeSocket(client_fd);

};