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

	std::thread tSendCommands(&Client::SendAccCommands, this, &end);
	tSendCommands.detach();

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

void Client::SendAccCommands(bool *end) //Sends accumulated commands once every 50ms for the server to validate them
{
	while (!*end)
	{
		if (!accCmd.accCommands.empty())
		{
			SendMessage(accCmd.GetCommandData());
			commandsToValidate.push_back(new AccCMD(accCmd));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void Client::SendCriticalMessage(Header header, std::string data)
{
	Header confirmationHeader = Header::COUNT;
	switch (header)
	{
	case Header::CONNECT:
		confirmationHeader = Header::CHALLENGE;
		break;
	case Header::CLIENT_DISCONNECT:
		confirmationHeader = Header::CLIENT_DISCONNECT_ACK;
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
		if (duration.count() >= 50)
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
			std::cout << dataReceived[2] << std::endl;

			player.playerNum = atoi(dataReceived[1].c_str()); // 1 means you are player 1 ; 2 means you are player 2

			other.playerNum = (atoi(dataReceived[1].c_str()) + 1) % 2; // the other player
			connected = true;
			break;
		case Header::SERVER_DISCONNECT:
			*end = true;
			connected = false;
			cleanDisconnect = true;
			std::cout << "Servidor desconectado\nPULSE CUALQUIER TECLA Y ENTER PARA CERRAR";

			break;
		case Header::CLIENT_DISCONNECT_ACK:
			*end = true;
			connected = false;
			cleanDisconnect = true;
			std::cout << "Desconectado por inactividad\nPULSE CUALQUIER TECLA Y ENTER PARA CERRAR";
			break;
		case Header::OK_ACTION:
			for (int i = 0; i < commandsToValidate.size(); i++)
			{
				if (commandsToValidate[i]->id == atoi(dataReceived[1].c_str()))
				{
					if(atoi(dataReceived[2].c_str()) % 2 == 0)
					{
						player.position.x = atoi(dataReceived[3].c_str()); player.position.y = atoi(dataReceived[4].c_str());

					}
					else
					{
					std::vector<AccCMD*>::iterator it1, it2;
					it1 = commandsToValidate.begin();
					it2 = commandsToValidate.begin() + i;
					commandsToValidate.erase(it1,it2);
					}
					break;
				}
			}
			break;
		case Header::MESSAGE:
			std::cout << dataReceived[1] << std::endl;
			break;
		default:
			break;
		}
	}
}