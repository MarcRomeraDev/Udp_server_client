#include "Client.h"
#include <iostream>



Client::Client(std::vector<std::string>& messages)
{

	socket = new UdpSocket();
	recipient = socket->GetLocalHost();

	if (!socket->Bind())
	{
		std::cout << "ERROR AL CONECTARSE AL PUERTO" << std::endl;
	}

	//std::string message = "Mi IP: " + socket->GetLocalIP() + "\nMi Puerto: " + std::to_string(socket->GetLocalPort());
	std::string message = "Inserta tu nickname";
	messages.push_back(message); //--> Demands the nickname through the interface

	

//if (!socket->Send(message.c_str(), message.size() + 1, recipient, 55002))
//{
//	std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
//}
//if (!socket->Receive(data, sizeof(data), received, sender, port))
//{
//	std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
//}

	//messages.push_back(data); //--> Output Server's welcome message to the interface screen
	std::thread tRecieve(&Client::ReceiveMessages, this, &messages, &end); //--> Thread to wait for incoming messages of other clients sent from server
	tRecieve.detach();

}


Client::~Client()
{
	end = true;
	delete socket;
}

void Client::SendMessage(std::string message)
{
	if (!socket->Send(message.c_str(), message.size() + 1, recipient, 55002))
	{
		std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
	}
}

void Client::ReceiveMessages(std::vector<std::string>* messages, bool *end)
{
	while (!end)
	{
		if (!socket->Receive(data, sizeof(data), received, sender, port))
		{
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
		}

		messages->push_back(data);
	}


}