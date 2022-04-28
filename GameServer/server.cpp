#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <unordered_set>

void ManageConnections(sf::UdpSocket& _socket, bool _end, std::unordered_set<unsigned short> _clients)
{
	std::string message = "";

	//Wait for icnoming messages
	while (!_end)
	{
		sf::IpAddress sender;
		unsigned short port;

		char data[1024] = "";
		std::size_t received = 0;

		if (_socket.receive(data, sizeof(data), received, sender, port) != sf::Socket::Done)
		{
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
			//_end = true;
		}

		std::cout << std::to_string(port) << " dice: " << data << std::endl;

		if (_clients.find(port) != _clients.end())
		{
			message = std::to_string(port) + " dice: " + data;
			//incoming message of an existing client, forward to all other clients
			for (const auto& elem : _clients)
			{
				if (elem != port)
				{
					if (_socket.send(message.c_str(), message.size() + 1, sender, elem) != sf::Socket::Done)
					{
						std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
					}
				}
			}
		}
		else
		{
			_clients.insert(port);
			message = "Bienvenido " + sender.toString();

			//Answer
			if (_socket.send(message.c_str(), message.size() + 1, sender, port) != sf::Socket::Done)
			{
				std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
			}
		}
	}
}

int main()
{
	sf::UdpSocket socket;
	bool end = false;
	std::unordered_set<unsigned short> clients;

	if (socket.bind(55002) != sf::Socket::Done)
	{
		std::cout << "ERROR AL BINDEAR AL PUERTO" << std::endl;
		return -1;
	}

	ManageConnections(socket, end, clients);

	return 0;
}