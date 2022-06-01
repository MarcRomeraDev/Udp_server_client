#pragma once
#include <iostream>
#include <unordered_map>
#include <UdpSocket.h>
#include <Types.h>
#include <PlayerInfo.h>

float CreateChallenge(std::string sender, unsigned short port, UdpSocket& socket) // recibe puerto destino
{
	std::string message;
	message = std::to_string((int)Header::CHALLENGE);
	message += "<CHALLENGE\n";
	int numero1, numero2;
	numero2 = (rand() % 100) + 1;
	numero1 = (rand() % 100) + 1 + numero2;

	if (rand() % 2 == 0)
	{
		message += std::to_string(numero1) + "+" + std::to_string(numero2);
	}
	else {
		message += std::to_string(numero1) + "-" + std::to_string(numero2);
		numero2 *= -1;
	}

	if (!socket.Send(message.c_str(), message.size() + 1, sender, port))
	{
		std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
	}

	return numero1 + numero2;
}

void ManageConnections(UdpSocket& socket, bool end, std::unordered_map<unsigned short, PlayerInfo*> clientsNoValidados, std::unordered_map<unsigned short, PlayerInfo*> clientsValidados)
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

		if (clientsValidados.find(port) != clientsValidados.end()) // Check if client already exist
		{
			//SWITCH INGAME
			// 
			//incoming message of an existing client, forward to all other clients
			//for (const auto& elem : clientsValidados)
			//{
			//	if (elem != port)
			//	{
			//		if (!socket.Send(message.c_str(), message.size() + 1, sender, elem))
			//		{
			//			std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
			//		}
			//	}
			//}
		}
		else if(clientsNoValidados.find(port) != clientsNoValidados.end())
		{
			std::cout << "CLIENTE NO VALIDADO AUN" << std::endl;
			//SWITCH PRE GAME
			//clientsNoValidados.at(port);
			// if validated
				//player.name = 
				//clientesNoValidados.at(port).delete
				//clientesValidados.insert(std::pair<unsigned short, PlayerInfo*>(port, player));
		}
		else // is new client
		{
			//ENVIAR CHALLENGE AL CLIENTE NO VALIDADO
			PlayerInfo* player = new PlayerInfo;
			player->challengeSolution = CreateChallenge(sender, port, socket);
			clientsNoValidados.insert(std::pair<unsigned short, PlayerInfo*>(port, player));
		}
	}
}

int main()
{
	srand(static_cast<unsigned>(time(nullptr)));

	UdpSocket* socket = new UdpSocket();
	// Crear salas
	bool end = false;
	std::unordered_map<unsigned short, PlayerInfo*> incommingClients;
	std::unordered_map<unsigned short, PlayerInfo*> validatedClients;

	if (!socket->Bind(55002))
	{
		std::cout << "ERROR AL BINDEAR AL PUERTO" << std::endl;
		delete socket;
		return -1;
	}

	ManageConnections(*socket, end, incommingClients, validatedClients);

	delete socket;
	return 0;
}