#pragma once
#include <iostream>
#include <unordered_set>
#include <UdpSocket.h>
#include <Types.h>
void ManageConnections(UdpSocket& socket, bool end, std::unordered_set<unsigned short> clients)
{
	std::string message = "";
	std::string sender;
	unsigned short port;
	char data[1024] = "";
	std::size_t received = 0;
	std::vector<std::string> dataReceived;
	//Wait for incoming messages
	while (!end)
	{
		if (!socket.Receive(data, sizeof(data), received, sender, port))
		{
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
			continue;
		}
		dataReceived.clear();
		std::string s(data);
		dataReceived = Split(s, '<');
		std::cout << port << " dice: " << dataReceived[0] << " Con la informacion: " << data << std::endl;

		if (clients.find(port) != clients.end()) // Check if client already exist
		{
			message = std::to_string(port) + " dice: " + data;
			//incoming message of an existing client, forward to all other clients
			for (const auto& elem : clients)
			{
				if (elem != port)
				{
					if (!socket.Send(message.c_str(), message.size() + 1, sender, elem))
					{
						std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
					}
				}
			}
		}
		else // is new client
		{
			clients.insert(port);
			
			//ENVIAR CHALLENGE AL CLIENTE NO VALIDADO

			if (!socket.Send(message.c_str(), message.size() + 1, sender, port))
			{
				std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
			}
		}
	}
}

int main()
{
	UdpSocket* socket = new UdpSocket();

	bool end = false;
	std::unordered_set<unsigned short> clients;


	if (!socket->Bind(55002))
	{
		std::cout << "ERROR AL BINDEAR AL PUERTO" << std::endl;
		delete socket;
		return -1;
	}

	ManageConnections(*socket, end, clients);

	delete socket;
	return 0;
}