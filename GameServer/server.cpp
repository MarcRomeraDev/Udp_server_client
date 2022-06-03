#pragma once
#include <iostream>
#include <unordered_map>
#include <UdpSocket.h>
#include <Types.h>
#include <PlayerInfo.h>

struct Game
{
	std::vector<PlayerInfo> players;
	int gameTime = 30;
};

int GetDistanceBetweenChars(char a, char b)
{
	int distance = abs(a - b);
	if (distance > 13)
	{
		distance = 26 - distance;
	}
	return distance;
}

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

void ManageConnections(UdpSocket& socket, bool end, std::unordered_map<unsigned short, PlayerInfo*> clientsNoValidados, std::unordered_map<unsigned short, PlayerInfo*> clientsValidados, std::vector<Game*> games)
{
	std::string message = "";
	std::string sender;
	const int MAX_ATTEMPS_PER_CLIENT = 3;
	unsigned short port;
	char data[1024] = "";
	std::size_t received = 0;
	std::vector<std::string> dataReceived;
	Header header;
	bool gameAvailable = false;

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
			switch (header)
			{
			case Header::CONNECT:
				std::cout << "CLIENTE VALIDADO CONECTADO" << std::endl;
				clientsValidados.at(port)->clientSalt = static_cast<uint32_t>(std::stoul(dataReceived[1].c_str()));
				message = std::to_string((int)Header::ACK_CHALLENGE); //CONNECTION APPROVED NOTIFICATION TO THE CLIENT
				message += "<WELCOME\n";

				if (!socket.Send(message.c_str(), message.size() + 1, sender, port))
				{
					std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
				}
				break;
			case Header::CLIENT_DISCONNECT:
				std::cout << "CLIENTE VALIDADO DESCONECTADO" << std::endl;
				message = std::to_string((int)Header::CLIENT_DISCONNECT_ACK); //CONNECTION APPROVED NOTIFICATION TO THE CLIENT
				message += "<Disconnecting...\n";
				if (!socket.Send(message.c_str(), message.size() + 1, sender, port))
				{
					std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
				}

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
				std::cout << "CLIENTE NO VALIDADO DESCONECTADO" << std::endl;
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
						

						//CREATE GAME OR JOIN EXISTING ONE WITH LESS THAN TWO PLAYERS
						if (games.size() > 0)
						{
							gameAvailable = false;
							for (const auto& match : games)
							{
								//If the game is not full and the distance between the first character of players' names is less than 10, join game
								if (match->players.size() < 2 && GetDistanceBetweenChars(match->players[0].name[0], clientsValidados.at(port)->name[0]) < 10)
								{
									std::unordered_map<unsigned short, PlayerInfo*>::iterator it = clientsValidados.find(port);
									match->players.push_back(*it->second);
									gameAvailable = true;
									message += "<1"; // le dice al cliente que el es el player 2
									break;
								}
							}
							if (!gameAvailable) //if all the items are full or the requirements are not met, create new game
							{
								Game* game = new Game;
								std::unordered_map<unsigned short, PlayerInfo*>::iterator it = clientsValidados.find(port);
								game->players.push_back(*it->second);
								games.push_back(game);
								message += "<0"; // le dice al cliente que el es el player 1
							}
						}
						else //No games exist, create new game
						{
							Game* game = new Game;
							std::unordered_map<unsigned short, PlayerInfo*>::iterator it = clientsValidados.find(port);
							game->players.push_back(*it->second);
							games.push_back(game);
							message += "<0"; // le dice al cliente que el es el player 1
						}
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
						{
							clientsNoValidados.at(port)->challengeSolution = CreateChallenge(sender, port, socket, std::to_string(clientsNoValidados.at(port)->serverSalt));
						}
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

	std::vector<Game*> games;

	if (!socket->Bind(55002))
	{
		std::cout << "ERROR AL BINDEAR AL PUERTO" << std::endl;
		delete socket;
		return -1;
	}

	ManageConnections(*socket, end, incommingClients, validatedClients, games);
	//Disconnect(*socket, validatedClients, incommingClients);
	delete socket;
	return 0;
}