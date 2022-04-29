#include "Client.h"
#include <iostream>

Client::Client(std::vector<std::string>& messages)
{
	//if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done)
	//{
	//	std::cout << "ERROR AL CONECTARSE AL PUERTO" << std::endl;
	//	//return;
	//}
	socket = new UdpSocket();

	if (!socket->Bind())
	{
		std::cout << "ERROR AL CONECTARSE AL PUERTO" << std::endl;
	}

	std::string message = "Mi IP: " + socket->GetLocalIP() + "\nMi Puerto: " + std::to_string(socket->GetLocalPort());

	//Sends message to server
	//if (socket.send(message.c_str(), message.size() + 1, recipient, 55002) != sf::Socket::Done)
	//{
	//	std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
	//	//return;
	//}

	if (!socket->Send(message.c_str(), message.size() + 1, recipient, 55002))
	{
		std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
	}

	//Receive answer
	//if (socket.receive(data, sizeof(data), received, sender, port) != sf::Socket::Done)
	//{
	//	std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
	//	//return;
	//}

	if (!socket->Receive(data, sizeof(data), received, sender, port))
	{
		std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
	}

	messages.push_back(data); //--> Output Server's welcome message to the interface screen

	std::thread tRecieve(&Client::RecieveMessages, this, &messages); //--> Thread to wait for incoming messages of other clients sent from server
	tRecieve.detach();
}

Client::~Client()
{
	delete socket;
}

void Client::SendMessage(std::string message)
{
	/*if (socket.send(message.c_str(), message.size() + 1, recipient, 55002) != sf::Socket::Done)
	{
		std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
	}*/

	if (!socket->Send(message.c_str(), message.size() + 1, recipient, 55002))
	{
		std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
	}
}

void Client::RecieveMessages(std::vector<std::string>* messages)
{
	bool end = false;

	while (!end)
	{
		/*if (socket.receive(data, sizeof(data), received, sender, port) != sf::Socket::Done)
		{
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
		}*/

		if (!socket->Receive(data, sizeof(data), received, sender, port))
		{
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
		}

		messages->push_back(data); //--> Output Server's welcome message to the interface screen
	}
}