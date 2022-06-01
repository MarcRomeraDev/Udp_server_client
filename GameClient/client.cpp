#include "Client.h"
#include <iostream>
#include <PlayerInfo.h>

Client::Client()
{
	socket = new UdpSocket();
	PlayerInfo player;
	recipient = socket->GetLocalHost();


	if (!socket->Bind())
	{
		std::cout << "ERROR AL CONECTARSE AL PUERTO" << std::endl;
		return;
	}
	std::cout << "Inserta tu nickname" << std::endl;
	std::cin >> player.name;

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
	std::thread tRecieve(&Client::ReceiveMessages, this, &end); //--> Thread to wait for incoming messages of other clients sent from server
	tRecieve.detach();

	Connect();
}

Client::~Client()
{
	end = true;
	delete socket;
}

void Client::Connect()
{
	std::string _message;
	_message = std::to_string((int)Header::CONNECT);
	_message += "<" + std::to_string(CreateSALT());

	std::thread tSendCritical(&Client::SendCriticalMessage, this, Header::CONNECT, _message);
	tSendCritical.detach();
}

//uint32_t Client::CreateSALT()
//{
//	uint32_t x = rand() & 0xff;
//	x |= (rand() & 0xff) << 8;
//	x |= (rand() & 0xff) << 16;
//	x |= (rand() & 0xff) << 24;
//
//	return x;
//}

void Client::SendCriticalMessage(Header header, std::string data)
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
	clock start = std::chrono::system_clock::now();
	clock end;
	std::chrono::duration<float, std::milli> duration;
	SendMessage(data);
	while (lastReceived != confirmationHeader)
	{
		end = std::chrono::system_clock::now();
		duration = end - start;
		if (duration.count() >= 500)
		{
			SendMessage(data);
			start = std::chrono::system_clock::now();
		}
	}
}

void Client::SendMessage(std::string message)
{
	if (!socket->Send(message.c_str(), message.size() + 1, recipient, 55002))
	{
		std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
	}
}

void Client::ReceiveMessages(bool* end)
{
	std::vector<std::string> dataReceived;
	std::string message;
	while (!*end)
	{
		if (!socket->Receive(data, sizeof(data), received, sender, port))
		{
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
			continue;
		}
		dataReceived.clear();
		std::string s(data);
		dataReceived = Split(s, '<');

		lastReceived = static_cast<Header>(atoi(dataReceived[0].c_str()));

		switch (lastReceived)
		{
		case Header::CHALLENGE:
			std::cout << dataReceived[1] << std::endl;
			break;
		default:
			break;
		}
	}
}