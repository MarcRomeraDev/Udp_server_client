#include "Client.h"
#include <iostream>

Client::Client()
{
	socket = new UdpSocket();

	recipient = socket->GetLocalHost();

	if (!socket->Bind())
	{
		std::cout << "ERROR AL CONECTARSE AL PUERTO" << std::endl;
		return;
	}
	std::cout << "Inserta tu nickname" << std::endl;
	std::cin >> player.name;

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
	player.clientSalt = CreateSALT();
	_message += "<" + std::to_string(player.clientSalt);

	std::thread tSendCritical(&Client::SendCriticalMessage, this, Header::CONNECT, _message);
	tSendCritical.detach();
}

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
			player.serverSalt = static_cast<uint32_t>(std::stoul(dataReceived[2].c_str()));
			player.saltTag = (player.clientSalt & player.serverSalt); //AND binario para generar el tag unico de cada cliente
			break;
		case Header::ACK_CHALLENGE:
			std::cout << dataReceived[1] << std::endl;
			connected = true;
		default:
			break;
		}
	}
}