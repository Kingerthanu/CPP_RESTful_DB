#include "client.h"
#include "socket.h"
#include <sstream>

Client::Client(const std::string& server_ip, const int& server_port)
{

	// Create Client Socket And Connect To Server
	this->socket_fd = Socket::createClientSocket(server_ip, server_port);

};

void Client::sendLine(const std::string& line)
{

	std::string data = line + "\n";
	Socket::sendData(this->socket_fd, data.c_str(), static_cast<int>(data.size()));

};

void Client::readLine(std::string& line)
{

	// Clear The Line Before Reading New Data
	line.clear();
	char buffer[1];

	while (true)
	{

		int bytes_received = Socket::receiveData(this->socket_fd, buffer, 1);
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

std::string Client::get(const std::string& key)
{

	// Send GET Command To Server
	std::ostringstream oss;
	oss << "GET " << key;
	sendLine(oss.str());

	// Read Response From Server
	std::string response;
	readLine(response);


	return response;

};

void Client::close()
{

	std::string demo = "555";

	// Close The Connection
	sendLine("EXIT");

};

Client::~Client()
{

	// Ensure The Socket Is Closed On Destruction
	this->close();

};

void Client::put(const std::string& key, const std::string& val)
{

	// Send PUT Command To Server
	std::ostringstream oss;
	oss << "PUT " << key << " " << val;
	sendLine(oss.str());

	// Read Response From Server
	std::string response;
	readLine(response);

};

void Client::beginTransaction()
{

	// Send BEGIN Command To Server
	sendLine("BEGIN");

	// Read Response From Server
	std::string response;
	readLine(response);

};

void Client::abortTransaction()
{

	// Send ABORT Command To Server
	sendLine("ABORT");

	// Read Response From Server
	std::string response;
	readLine(response);

};

void Client::commitTransaction()
{

	// Send COMMIT Command To Server
	sendLine("COMMIT");

	// Read Response From Server
	std::string response;
	readLine(response);

};