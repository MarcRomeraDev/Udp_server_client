#include "Client.h"
#include <iostream>

Client::Client(std::vector<std::string>& messages)
{
	if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
	{
		std::cout << "ERROR AL CONECTARSE AL PUERTO" << std::endl;
		//return;
	}

	std::string message = "Mi IP: " + sf::IpAddress::getLocalAddress().toString() + "\nMi Puerto: " + std::to_string(socket.getLocalPort());	

	//Sens message to server
	if (socket.send(message.c_str(), message.size() + 1, recipient, 55002) != sf::Socket::Done)
	{
		std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
		//return;
	}	

	//Recieve answer
	if (socket.receive(data, sizeof(data), received, sender, port) != sf::Socket::Done)
	{
		std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
		//return;
	}
	messages.push_back(data); //--> Output Server's welcome message to the interface screen

	std::thread tRecieve(&Client::RecieveMessages, this, &messages);
	tRecieve.detach();
}

void Client::SendMessage(std::string message)
{
	if (socket.send(message.c_str(), message.size() + 1, recipient, 55002) != sf::Socket::Done)
	{
		std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
	}
}

void Client::RecieveMessages(std::vector<std::string>* messages)
{
	bool end = false;
	char data[1024] = "";
	std::size_t received = 0;

	while (!end)
	{
		if (socket.receive(data, sizeof(data), received, sender, port) != sf::Socket::Done)
		{
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
		}
		
		messages->push_back(data); //--> Output Server's welcome message to the interface screen
	}
}