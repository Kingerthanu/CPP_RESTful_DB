
#include "broker.h"
#include <iostream>

void Broker::beginTransaction(const int& clientID)
{
    std::lock_guard<std::mutex> lock(mut_);
    pendingCommits[clientID] = std::unordered_map<std::string, std::string>();
    std::cout << "[DEBUG] Transaction started for client " << clientID << std::endl;
}

void Broker::abortTransaction(const int& clientID)
{
    std::lock_guard<std::mutex> lock(mut_);
    pendingCommits.erase(clientID);
    std::cout << "[DEBUG] Transaction aborted for client " << clientID << std::endl;
}

void Broker::commitTransaction(const int& clientID)
{
    std::lock_guard<std::mutex> lock(mut_);

    std::cout << "[DEBUG] Committing transaction for client " << clientID << std::endl;
    std::cout << "[DEBUG] Pending commits size: " << pendingCommits[clientID].size() << std::endl;

    // For Given clientID, Apply All Pending Commits To The Main Database
    for (const auto& commit : pendingCommits[clientID])
    {
        std::cout << "[DEBUG] Committing Key: " << commit.first << " Value: " << commit.second << std::endl;
        db_[commit.first] = commit.second;
    }

    std::cout << "[DEBUG] db_ size after commit: " << db_.size() << std::endl;

    // Remove The Map From Pending Commits
    pendingCommits.erase(clientID);
}

void Broker::put(const std::string& key, const std::string& val, const int& clientID)
{
    std::lock_guard<std::mutex> lock(mut_);

    std::cout << "[DEBUG] PUT - Client: " << clientID << ", Key: " << key << ", Value: " << val << std::endl;

    // Check if transaction exists
    if (pendingCommits.find(clientID) == pendingCommits.end())
    {
        std::cout << "[DEBUG] WARNING: No active transaction for client " << clientID << std::endl;
    }

    pendingCommits[clientID][key] = val;
    std::cout << "[DEBUG] Pending commits size for client " << clientID << ": " << pendingCommits[clientID].size() << std::endl;
}

std::string Broker::get(const std::string& key, const int& clientID)
{
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
}

void Broker::saveToFile(const std::string& fileName)
{
    std::cout << "[DEBUG] Saving database to file: " << fileName << std::endl;

    std::lock_guard<std::mutex> lock(mut_);

    std::cout << "[DEBUG] db_ size at save time: " << db_.size() << std::endl;

    std::ofstream outFile(fileName, std::ios::trunc);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Failed to open file for saving database.");
    }

    // Write Key-Value Pairs To File
    for (const auto& pair : db_)
    {
        std::cout << "[DEBUG] Saving Key: " << pair.first << " Value: " << pair.second << std::endl;
        outFile << pair.first << "=" << pair.second << std::endl;
    }
    outFile.close();

    std::cout << "[DEBUG] Database saved successfully." << std::endl;
}