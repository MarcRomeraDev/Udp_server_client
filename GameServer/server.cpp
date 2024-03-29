#pragma once
#include <iostream>
#include <unordered_map>
#include <UdpSocket.h>
#include <Types.h>
#include <PlayerInfo.h>
#include <thread>
#include <mutex>
#include <Command.h>
#include <Constants.h>
#include <queue>

#define TIME_FOR_VALIDATED_DISCONECTION 30000
#define TIME_FOR_NOT_VALIDATED_DISCONECTION 30000
#define DISCONECTION_REFRESH_TIME 5
#define CONFIRMATION_REFRESH_TIME 50

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

void Disconnect(UdpSocket& _socket, std::unordered_map<unsigned short, PlayerInfo*>& clientesValidados, std::unordered_map<unsigned short, PlayerInfo*>& clientesNoValidados)
{
	std::string message;
	message = std::to_string((int)Header::SERVER_DISCONNECT);
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

void ManageDisconnections(UdpSocket* socket, bool* end, std::unordered_map<unsigned short, PlayerInfo*>* clientesNoValidados, std::unordered_map<unsigned short, PlayerInfo*>* clientesValidados, std::vector<Game*>* games, std::mutex* mtx)
{
	typedef std::chrono::system_clock::time_point clock;
	clock start;

	std::chrono::duration<float, std::milli> duration;
	std::string message;
	message = std::to_string((int)Header::CLIENT_DISCONNECT_ACK);

	while (!*end)
	{
		start = std::chrono::system_clock::now();

		for (auto it = clientesValidados->begin(); it != clientesValidados->end();)
		{
			duration = start - it->second->timeStamp;
			if (duration.count() > TIME_FOR_VALIDATED_DISCONECTION)
			{
				if (!socket->Send(message.c_str(), message.size() + 1, it->second->ip, it->second->port))
				{
					std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
				}
				std::cout << "CLIENTE INACTIVO CON PUERTO: " << it->first << "DESCONECTADO" << std::endl;
				mtx->lock();
				delete clientesValidados->at(it->first);
				it = clientesValidados->erase(it);
				mtx->unlock();
			}
			else
			{
				++it;
			}
		}

		for (auto it = clientesNoValidados->begin(); it != clientesNoValidados->end();)
		{
			duration = start - it->second->timeStamp;
			if (duration.count() > TIME_FOR_NOT_VALIDATED_DISCONECTION)
			{
				if (!socket->Send(message.c_str(), message.size() + 1, it->second->ip, it->second->port))
				{
					std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
				}
				std::cout << "CLIENTE INACTIVO CON PUERTO: " << it->first << "DESCONECTADO" << std::endl;
				mtx->lock();
				delete clientesNoValidados->at(it->first);
				it = clientesNoValidados->erase(it);
				mtx->unlock();
			}
			else
			{
				++it;
			}
		}

		// Dormir el thread
		std::this_thread::sleep_for(std::chrono::seconds(DISCONECTION_REFRESH_TIME));
	}
}

void DisconnectServer(bool* end, UdpSocket* socket, std::unordered_map<unsigned short, PlayerInfo*>* clientesNoValidados, std::unordered_map<unsigned short, PlayerInfo*>* clientesValidados, std::vector<Game*>* games, std::mutex* mtx)
{
	int a;
	std::cin >> a; // Si pulsas enter se cierra el servidor
	*end = true;
	Disconnect(*socket, *clientesValidados, *clientesNoValidados);
	delete socket;
}

bool ValidateMovement(int x, int y)
{
	return(x * SIZE > W_WINDOW_PX || x * SIZE < 0 || y * SIZE > H_WINDOW_PX || y * SIZE < 0);
}

void ValidateCommands(UdpSocket* socket, bool* end, std::queue<Command*>* commandsToValidate, std::unordered_map<unsigned short, PlayerInfo*>* clientesValidados) // Can only be Move or Shoot
{
	std::vector<std::string> dataReceived;
	Header header;
	int direction = 0;
	std::string message;
	while (!*end)
	{
		while (!commandsToValidate->empty())
		{
			Command* command = commandsToValidate->front();
			commandsToValidate->pop();

			dataReceived.clear();
			std::string s(command->data);
			dataReceived = Split(s, '<');
			bool val = false;

			for (int i = 0; i < dataReceived.size(); i++)
			{
				header = static_cast<Header>(atoi(dataReceived[i].c_str()));
				i++;
				switch (header)
				{
				case Header::MOVE:
					direction = atoi(dataReceived[i].c_str());
					i++;
					switch (direction)
					{
					case 0: //up
						clientesValidados->at(command->senderPort)->position.y--;
						break;
					case 1: //down
						clientesValidados->at(command->senderPort)->position.y++;
						break;
					case 2: //left
						clientesValidados->at(command->senderPort)->position.x--;
						break;
					case 3: // right
						clientesValidados->at(command->senderPort)->position.x++;
						break;
					default:
						break;
					}

					message = std::to_string((int)Header::OK_ACTION);
					message += "<" + std::to_string(command->id);
					val = ValidateMovement(clientesValidados->at(command->senderPort)->position.x, clientesValidados->at(command->senderPort)->position.y);
					message += "<" + std::to_string(val);
					if (!val)
					{
						switch (direction)
						{
						case 0: //up
							clientesValidados->at(command->senderPort)->position.y++;
							break;
						case 1: //down
							clientesValidados->at(command->senderPort)->position.y--;
							break;
						case 2: //left
							clientesValidados->at(command->senderPort)->position.x++;
							break;
						case 3: // right
							clientesValidados->at(command->senderPort)->position.x--;
							break;
						default:
							break;
						}
						message += "<" + std::to_string(clientesValidados->at(command->senderPort)->position.x) + "<" + std::to_string(clientesValidados->at(command->senderPort)->position.y);
					}
					if (!socket->Send(message.c_str(), message.size() + 1, command->ipAddress, command->senderPort));
					{
						std::cout << "ERROR AL ENVIAR PACKET" << std::endl;
					}

					break;
				case Header::SHOOT:
					break;
				default:
					break;
				}
			}
		}

		// Dormir el thread
		std::this_thread::sleep_for(std::chrono::seconds(CONFIRMATION_REFRESH_TIME));
	}
}

void ManageConnections(UdpSocket& socket, bool end, std::unordered_map<unsigned short, PlayerInfo*> clientsNoValidados, std::unordered_map<unsigned short, PlayerInfo*> clientsValidados, std::vector<Game*> games)
{
	std::string message = "";
	std::string sender;
	std::queue<Command*> commandsToValidate;
	const int MAX_ATTEMPS_PER_CLIENT = 3;
	unsigned short port;
	char data[1024] = "";
	std::size_t received = 0;
	std::vector<std::string> dataReceived;
	Header header;
	bool gameAvailable = false;
	std::mutex mtx;

	std::thread tDisconnectServer(DisconnectServer, &end, &socket, &clientsNoValidados, &clientsValidados, &games, &mtx);
	tDisconnectServer.detach();

	std::thread tDisconnections(ManageDisconnections, &socket, &end, &clientsNoValidados, &clientsValidados, &games, &mtx);
	tDisconnections.detach();

	std::thread tValidateCommands(ValidateCommands, &socket, &end, &commandsToValidate, &clientsValidados);
	tValidateCommands.detach();

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
		mtx.lock();
		if (clientsValidados.find(port) != clientsValidados.end()) // Check if client already exist
		{
			clientsValidados.at(port)->timeStamp = std::chrono::system_clock::now();

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
				break;
			case Header::COMMAND:
				//clientsValidados.at(port)->clientSalt = static_cast<uint32_t>(std::stoul(dataReceived[1].c_str()));
				//message = std::to_string((int)Header::ACK_CHALLENGE); //CONNECTION APPROVED NOTIFICATION TO THE CLIENT
				commandsToValidate.push(new Command());
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
			clientsNoValidados.at(port)->timeStamp = std::chrono::system_clock::now();

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
									clientsValidados.at(port)->position.x = W_WINDOW_TITLE - 1; clientsValidados.at(port)->position.y = H_WINDOW_TITLE - 1;
									break;
								}
							}
							if (!gameAvailable) //if all the items are full or the requirements are not met, create new game
							{
								Game* game = new Game;
								std::unordered_map<unsigned short, PlayerInfo*>::iterator it = clientsValidados.find(port);
								game->players.push_back(*it->second);
								games.push_back(game);
								clientsValidados.at(port)->position.x = 0; clientsValidados.at(port)->position.y = 0;
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
			player->timeStamp = std::chrono::system_clock::now();
			clientsNoValidados.insert({ port, player });

			player->challengeSolution = CreateChallenge(sender, port, socket, std::to_string(player->serverSalt));
		}
		mtx.unlock();
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

	std::vector<Game*> games;

	if (!socket->Bind(55002))
	{
		std::cout << "ERROR AL BINDEAR AL PUERTO" << std::endl;
		delete socket;
		return -1;
	}

	ManageConnections(*socket, end, incommingClients, validatedClients, games);

	//Disconnect(*socket, validatedClients, incommingClients);
	//delete socket;
	return 0;
}