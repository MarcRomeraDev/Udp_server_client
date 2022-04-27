#include "Client.h"
#include <iostream>

Client::Client()
{
	sf::UdpSocket socket; //--> Desvincular libreria de red
	if (socket.bind(55001) != sf::Socket::Done)
	{
		std::cout << "ERROR AL CONECTARSE AL PUERTO" << std::endl;
		return;
	}

	std::string message = "Mi IP: " + sf::IpAddress::getLocalAddress().toString();

	sf::Packet packet;
	packet << message;

	if (socket.send(packet, sf::IpAddress::LocalHost, 55002) != sf::Socket::Done)
	{
		std::cout << "ERROR AL ENVIAR PACKETE" << std::endl;
		return;
	}

	//conectarse al server

	//recibir vector de mensajes para actualizar interfaz	

	//Thread esperar mensajes
}