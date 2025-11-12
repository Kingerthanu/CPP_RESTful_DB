#pragma once

#include <string>
#include "broker.h"


class Server
{

	private:
		int server_fd;
		int port_fd;
		bool running;

		void handleClient(const int client_fd);
		void sendLine(int client_fd, const std::string& line);
		void readLine(int client_fd, std::string& line);

		Broker broker;

	
	public:
		Server(const int& port);
		~Server();

		void start();
		void stop();
		void saveDatabase(const std::string& fileName);
		void setRunning(const bool& state) { this->running = state; };

};