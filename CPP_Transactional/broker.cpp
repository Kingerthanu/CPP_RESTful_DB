
#include "broker.h"

void Broker::beginTransaction(const int& clientID) 
{

	// Lock Up The Mutex To Ensure One Thread Accessing Pending Commits At A Time
	std::lock_guard<std::mutex> lock(mut_);

	// For Given clientID, Create An Empty Map In Pending Commits
	pendingCommits[clientID] = std::unordered_map<std::string, std::string>();

};

void Broker::abortTransaction(const int& clientID) 
{

	// Lock Up The Mutex To Ensure One Thread Acessssing Pending Commits At A Time
	std::lock_guard<std::mutex> lock(mut_);

	// For Given clientID, Remove The Map From Pending Commits
	pendingCommits.erase(clientID);

};

void Broker::commitTransaction(const int& clientID)
{

	// Lock Up The Mutex To Ensure One Thread Accessing Pending Commits At A Time
	std::lock_guard<std::mutex> lock(mut_);

	// For Given clientID, Apply All Pending Commits To The Main Database
	for (const auto& commit : pendingCommits[clientID]) 
	{
		db_[commit.first] = commit.second;
	}

	// Remove The Map From Pending Commits
   	pendingCommits.erase(clientID);

};

void Broker::put(const std::string& key, const std::string& val, const int& clientID)
{

	// Lock Up The Mutex To Ensure Our Thread Is The Only One Accessing Pending Commits At A Time
	std::lock_guard<std::mutex> lock(mut_);

	// For Given clientID, Store The Key-Value Pair In Pending Commits
	pendingCommits[clientID][key] = val;

};

std::string Broker::get(const std::string& key, const int& clientID)
{

	// Ensure Thread Safety While Accessing The Main Database
	std::lock_guard<std::mutex> lock(mut_);

	// Fetch Item From Pending First
	if (pendingCommits.find(clientID) != pendingCommits.end() && pendingCommits[clientID].find(key) != pendingCommits[clientID].end()) 
	{
		return pendingCommits[clientID][key];
	}
	// If Not Found In Pending, Fetch From Main Database
	else if (db_.find(key) != db_.end()) 
	{
		return db_[key];
	} 
	else 
	{
		return ""; // Return Empty String If Key Not Found
	}

};

void Broker::saveToFile(const std::string& fileName)
{

	// Lock Control For Thread Safety
	std::lock_guard<std::mutex> lock(mut_);

	// Open/Create File To Write
	std::ofstream outFile(fileName, std::ios::trunc);
	if (!outFile.is_open()) 
	{
		throw std::runtime_error("Failed to open file for saving database.");
	}

	// Write Key-Value Pairs To File
	for (const auto& pair : db_) 
	{
		outFile << pair.first << "=" << pair.second << std::endl;
	}
	outFile.close();

};