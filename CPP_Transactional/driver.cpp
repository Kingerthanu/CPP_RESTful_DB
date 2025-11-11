#include "client.h"
#include "socket.h"
#include "server.h"
#include <iostream>


int main()
{
    try
    {

        std::cout << "\nStarting Initialization:" << std::endl;
		std::cout << "=====================================" << std::endl;
        std::cout << "Starting server..." << std::endl;
        Server server(8080);
        std::thread serverThread([&server]() { server.start(); });

        std::cout << "Waiting for server to start..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

		std::cout << "====================================\n\n\n";


        Client client("127.0.0.1", 8080);
        std::cout << "Client connected." << std::endl;

        client.beginTransaction();
        
        for (unsigned int indx = 1; indx < 100; indx++)
        {
			std::cout << "Inserting Key-Val Of: " << "key" + std::to_string(indx) << " : " << std::to_string(indx) << std::endl;
			client.put("key" + std::to_string(indx), std::to_string(indx));
        }

        client.commitTransaction();

        std::string value1 = client.get("key1");
        std::string value2 = client.get("key2");
        std::cout << "key1: " << value1 << std::endl;
        std::cout << "key2: " << value2 << std::endl;


		server.saveDatabase("database.txt");
		server.stop();
        if (serverThread.joinable()) {
            serverThread.join();
        }

        std::cout << "Program completed successfully!\n\n";

    }
    catch (const std::exception& e)
    {

        std::cerr << "Error: " << e.what() << std::endl;
        return 1;

    }

    return 0;
}
