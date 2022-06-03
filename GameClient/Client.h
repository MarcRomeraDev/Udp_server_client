#pragma once
#include <thread>
#include <UdpSocket.h>
#include <Types.h>
#include <PlayerInfo.h>
#include <AccCMD.h>

class Client
{
private:
	UdpSocket* socket;
	std::string recipient;
	std::string sender;
	bool end = false;
	unsigned short port;
	std::size_t received = 0;
	char data[1024] = "";
	typedef std::chrono::system_clock::time_point clock;

	void ReceiveMessages(bool*);
	void SendAccCommands(bool*);
	void Connect();

public:
	Client();
	~Client();
	Header lastReceived;
	bool connected = false;
	bool cleanDisconnect = false;
	PlayerInfo player;
	PlayerInfo other;
	AccCMD accCmd;
	std::vector<AccCMD> commandsToValidate;

	void SendCriticalMessage(Header header, std::string data);
	void SendMessage(std::string);
};