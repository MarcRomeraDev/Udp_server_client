#include "Client.h"
#include <iostream>
#include <PlayerInfo.h>



Client::Client()
{
	socket = new UdpSocket();
	recipient = socket->GetLocalHost();
	PlayerInfo player;
	if (!socket->Bind())
	{
		std::cout << "ERROR AL CONECTARSE AL PUERTO" << std::endl;
		return;
	}
	std::cout << "Inserta tu nickname" << std::endl;
	std::cin >> player.name;
	
	Connect();
	//std::string message = "Mi IP: " + socket->GetLocalIP() + "\nMi Puerto: " + std::to_string(socket->GetLocalPort());
	//std::string message = "Inserta tu nickname";
	//messages.push_back(message); //--> Demands the nickname through the interface

	

//if (!socket->Send(message.c_str(), message.size() + 1, recipient, 55002))
//{
//	std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
//}
//if (!socket->Receive(data, sizeof(data), received, sender, port))
//{
//	std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
//}

	//messages.push_back(data); //--> Output Server's welcome message to the interface screen
	//std::thread tRecieve(&Client::ReceiveMessages, this, &messages, &end); //--> Thread to wait for incoming messages of other clients sent from server
	//tRecieve.detach();

}



Client::~Client()
{
	end = true;
	delete socket;
}

void Client::Connect()
{
	std::string _message;
	_message = static_cast<std::string>((int)Header::CONNECT);
	_message += CreateSALT();
	SendCriticalMessage(Header::CONNECT,_message);
}

uint32_t Client::CreateSALT()
{
	uint32_t x = rand() & 0xff;
	x |= (rand() & 0xff) << 8;
	x |= (rand() & 0xff) << 16;
	x |= (rand() & 0xff) << 24;

	return x;
}

void Client::SendCriticalMessage(Header header,std::string data)
{
	Header confirmationHeader = Header::COUNT;
	switch (header)
	{
	case Header::CONNECT:
		confirmationHeader = Header::CHALLENGE;
		break;
	case Header::ACTION:
		confirmationHeader = Header::OK_ACTION;
		break;
	default:
		std::cout << "Error" << std::endl;
		break;
	}

	//Inicializamos temporizador
	while (lastReceived != confirmationHeader)
	{
		// if temporizador <= 0
			SendMessage(data);
			//Temporizador == maxRetryDelay
	}
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
			//Sacar el header de data TO DO
			//lastReceived = data;
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
		}

		messages->push_back(data);
	}


}