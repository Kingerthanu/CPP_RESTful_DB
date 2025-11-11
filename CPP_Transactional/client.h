#pragma once
#include <string>


class Client
{

	private:
		int socket_fd;
		void sendLine(const std::string& line);
		void readLine(std::string& line);

	public:
		Client(const std::string& server_ip, const int& server_port);
		std::string get(const std::string& key);
		void put(const std::string& key, const std::string& val);
		void beginTransaction();
		void abortTransaction();
		void commitTransaction();
		void close();
		~Client();

};