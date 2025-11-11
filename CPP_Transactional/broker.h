#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <fstream>


class Broker
{

	private:
		std::unordered_map<std::string, std::string> db_;
		std::unordered_map<int, std::unordered_map<std::string, std::string>> pendingCommits;
		std::mutex mut_;

	public:
		std::string get(const std::string& key, const int& clientID);
		void put(const std::string& key, const std::string& val, const int& clientID);

		void beginTransaction(const int& clientID);
		void abortTransaction(const int& clientID);
		void commitTransaction(const int& clientID);
		void saveToFile(const std::string& fileName);


};
