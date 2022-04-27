#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>

void ManageConnections(sf::UdpSocket& _socket, bool _end)
{
	sf::Packet packet;
	std::string message = "";
	
	//Recibir conexiones
	while (!_end)
	{
		packet.clear();

		/*char buffer[1024];
		std::size_t received = 0;*/
		sf::IpAddress sender;
		unsigned short port;

		if (_socket.receive(packet, sender, port) != sf::Socket::Done)
		{
			std::cout << "ERROR AL RECIVIR PACKET" << std::endl;
			_end = true;
		}

		packet >> message;
	}
}

int main()
{
	sf::UdpSocket socket;
	bool end = false;

	if (socket.bind(55002) != sf::Socket::Done)
	{
		std::cout << "ERROR AL BINDEAR AL PUERTO" << std::endl;
		return -1;
	}

	ManageConnections(socket, end);

	return 0;
}