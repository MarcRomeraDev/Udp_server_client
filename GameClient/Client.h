#pragma once
#include <thread>
#include <UdpSocket.h>
#include <Types.h>
#include <chrono>

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
	void Connect();
	void SendCriticalMessage(Header header, std::string data);

public:
	Client();
	~Client();
	Header lastReceived;
	bool connected = false; 

	void SendMessage(std::string);
};