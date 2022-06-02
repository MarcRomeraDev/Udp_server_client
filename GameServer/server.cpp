#pragma once
#include <iostream>
#include <unordered_map>
#include <UdpSocket.h>
#include <Types.h>
#include <PlayerInfo.h>

float CreateChallenge(std::string sender, unsigned short port, UdpSocket& socket, std::string salt) // recibe puerto destino
{
	std::string message;
	message = std::to_string((int)Header::CHALLENGE);
	message += "<CHALLENGE\n";
	int numero1, numero2;
	numero2 = (rand() % 100) + 1;
	numero1 = (rand() % 100) + 1 + numero2;

	if (rand() % 2 == 0) //Sum operation
	{
		message += std::to_string(numero1) + "+" + std::to_string(numero2);
	}
	else //Substract operation
	{
		message += std::to_string(numero1) + "-" + std::to_string(numero2);
		numero2 *= -1;
	}

	message += "<" + salt;
	//Send challenge
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
	const int MAX_ATTEMPS_PER_CLIENT = 3;
	unsigned short port;
	char data[1024] = "";
	std::size_t received = 0;
	std::vector<std::string> dataReceived;
	Header header;

	while (!end)
	{
		//Wait for incoming messages	
		if (!socket.Receive(data, sizeof(data), received, sender, port))
		{
			std::cout << "ERROR AL RECIBIR PACKET" << std::endl;
			continue;
		}
		dataReceived.clear();
		std::string s(data);
		dataReceived = Split(s, '<');

		std::cout << port << " dice: " << dataReceived[0] << " Con la informacion: " << data << std::endl;

		header = static_cast<Header>(atoi(dataReceived[0].c_str()));

		if (clientsValidados.find(port) != clientsValidados.end()) // Check if client already exist
		{
			std::cout << "CLIENTE VALIDADO" << std::endl;
			switch (header)
			{
			case Header::CONNECT:
				clientsValidados.at(port)->clientSalt = static_cast<uint32_t>(std::stoul(dataReceived[1].c_str()));
				message = std::to_string((int)Header::ACK_CHALLENGE); //CONNECTION APPROVED NOTIFICATION TO THE CLIENT
				message += "<WELCOME\n";

				if (!socket.Send(message.c_str(), message.size() + 1, sender, port))
				{
					std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
				}
				break;
			default:
				break;
			}
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
		else if (clientsNoValidados.find(port) != clientsNoValidados.end())
		{
			switch (header)
			{
			case Header::CONNECT:
				clientsNoValidados.at(port)->attemptCount = 1;
				clientsNoValidados.at(port)->clientSalt = static_cast<uint32_t>(std::stoul(dataReceived[1].c_str()));
				clientsNoValidados.at(port)->challengeSolution = CreateChallenge(sender, port, socket, std::to_string(clientsNoValidados.at(port)->serverSalt));
				break;
			case Header::CLIENT_DISCONNECT:
				message = std::to_string((int)Header::CLIENT_DISCONNECT_ACK); //CONNECTION APPROVED NOTIFICATION TO THE CLIENT
				message += "<Disconnecting...\n";
				if (!socket.Send(message.c_str(), message.size() + 1, sender, port))
				{
					std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
				}

				break;
			case Header::RSP_CHALLENGE:
				if ((clientsNoValidados.at(port)->clientSalt & clientsNoValidados.at(port)->serverSalt) == static_cast<uint32_t>(std::stoul(dataReceived[1])))
				{
					if (clientsNoValidados.at(port)->challengeSolution == atoi(dataReceived[2].c_str()))
					{
						clientsNoValidados.at(port)->name = dataReceived[3];
						clientsValidados.insert({ port, clientsNoValidados[port] });
						clientsNoValidados.erase(port);

						message = std::to_string((int)Header::ACK_CHALLENGE); //CONNECTION APPROVED NOTIFICATION TO THE CLIENT
						message += "<WELCOME\n";

						if (!socket.Send(message.c_str(), message.size() + 1, sender, port))
						{
							std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
						}
					}
					else
					{
						std::cout << "CHALLENGE INCORRECTO" << std::endl;
						if (clientsNoValidados.at(port)->attemptCount >= MAX_ATTEMPS_PER_CLIENT)
						{
							//Desconectar Cliente
							message = std::to_string((int)Header::CLIENT_DISCONNECT_ACK); //CONNECTION APPROVED NOTIFICATION TO THE CLIENT
							message += "<Too many attemps, disconnecting...\n";
							if (!socket.Send(message.c_str(), message.size() + 1, sender, port))
							{
								std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
							}
						}
						else
							clientsNoValidados.at(port)->challengeSolution = CreateChallenge(sender, port, socket, std::to_string(clientsNoValidados.at(port)->serverSalt));
						clientsNoValidados.at(port)->attemptCount++;
					}
				}
				else
				{
					std::cout << "TAG NO COINCIDE CON LA DEL CLIENTE ORIGINAL" << std::endl;
					//desconectar tramposo
				}
				break;
			default:
				break;
			}
		}
		else // is new client
		{
			//ENVIAR CHALLENGE AL CLIENTE NO VALIDADO
			PlayerInfo* player = new PlayerInfo;
			player->clientSalt = static_cast<uint32_t>(std::stoul(dataReceived[1].c_str()));
			player->serverSalt = CreateSALT();
			player->port = port;
			player->ip = sender;
			clientsNoValidados.insert({ port, player });

			player->challengeSolution = CreateChallenge(sender, port, socket, std::to_string(player->serverSalt));
		}
	}
}
void Disconnect(UdpSocket& _socket, std::unordered_map<unsigned short, PlayerInfo*>& clientesValidados, std::unordered_map<unsigned short, PlayerInfo*>& clientesNoValidados)
{
	std::string message;
	message = std::to_string((int)Header::CLIENT_DISCONNECT_ACK);
	for (const auto& elem : clientesValidados)
	{
		if (!_socket.Send(message.c_str(), message.size() + 1, elem.second->ip, elem.second->port))
		{
			std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
		}
		delete clientesValidados.at(elem.first);
	}
	for (const auto& elem : clientesNoValidados)
	{
		if (!_socket.Send(message.c_str(), message.size() + 1, elem.second->ip, elem.second->port))
		{
			std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
		}
		delete clientesNoValidados.at(elem.first);
	}
	clientesNoValidados.clear();
	clientesValidados.clear();
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
	Disconnect(*socket, validatedClients, incommingClients);
	delete socket;
	return 0;
}