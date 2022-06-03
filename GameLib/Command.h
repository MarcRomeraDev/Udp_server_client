#pragma once
#include <iostream>
#include "Types.h"

class Command
{
private:
	
public:
	Command();
	Command(std::string _data,unsigned short _senderPort, unsigned int _id, std::string);
	unsigned short senderPort;
	std::string ipAddress;
	unsigned int id;
	std::string data;
};