#include "client.h"
#include "socket.h"
#include "server.h"
#include "settings.h"
#include <iostream>
#include <csignal>

// Bool For Stopping Server
Server* serverInstance;

void signalHandler(int signum)
{
	std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
	if (serverInstance != nullptr)
	{
		serverInstance->stop();
	}
}

void startServer(const int& port)
{

    try
    { 
        std::cout << "\nStarting Initialization:" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "Starting server..." << std::endl;

        Server server(8080);
		serverInstance = &server;
    
	    std::cout << "Server Started On IP: (" << Settings::Networking::SERVER_IP << ") Port: (" << Settings::Networking::SERVER_PORT << ")\n";
        std::cout << "=====================================" << std::endl;

		signal(SIGINT, signalHandler);
        server.start();

	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}

}

void startClient(const std::string& server_ip, const int& server_port)
{

    try
    { 
 
	    std::cout << "\nStarting Client Connection:" << std::endl;
        std::cout << "=====================================" << std::endl;
	    Client client(server_ip, server_port); 
	    std::cout << "Client connected." << std::endl;
        std::cout << "=====================================" << std::endl;

        
        // Keep Looping Until User Exits
		std::string command;
        while (command != "EXIT")
        {
			std::cout << "Query > ";
			std::cin >> command;

			if (command == "GET")
			{
				std::string key;
				std::cin >> key;
				std::string value = client.get(key);
				std::cout << "Value: " << value << std::endl;
			}
			else if (command == "PUT")
			{
				std::string key, value;
				std::cin >> key >> value;
				client.put(key, value);
				std::cout << "PUT operation completed." << std::endl;
			}
			else if (command == "BEGIN")
			{
				client.beginTransaction();
				std::cout << "Transaction started." << std::endl;
			}
			else if (command == "ABORT")
			{
				client.abortTransaction();
				std::cout << "Transaction aborted." << std::endl;
			}
			else if (command == "COMMIT")
			{
				client.commitTransaction();
				std::cout << "Transaction committed." << std::endl;
			}
			else if (command == "EXIT")
			{
				std::cout << "Exiting client." << std::endl;
			}
			else
			{
				std::cout << "Unknown command. Available commands: GET, PUT, BEGIN, ABORT, COMMIT, EXIT" << std::endl;
			}
		}

		client.close();

    }
    catch (const std::exception& e)
    {
		std::cerr << "Error: " << e.what() << std::endl;
    }

}


// Take In Arguments
int main(int argc, char* argv[])
{

	if (argc > 1)
	{
		std::cout << "\nCPP_Transactional Application Starting With Arguments..." << std::endl;

		if(std::string(argv[1]) == "server")
        { 
			startServer(Settings::Networking::SERVER_PORT);
        }
        else if (std::string(argv[1]) == "client")
        {
			startClient(Settings::Networking::SERVER_IP, Settings::Networking::SERVER_PORT);
        }
        else
        {
			std::cout << "Invalid Argument. Use 'server' or 'client'.\n" << std::endl;
            return 1;
        }

	}
    else
    {
		std::cout << "\nNo Argument Provided. Please specify 'server' or 'client'.\n" << std::endl;
        return 1;
    }

    return 0;

}
