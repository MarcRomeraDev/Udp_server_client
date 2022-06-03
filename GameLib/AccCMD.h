#pragma once
#include <vector>
#include "Command.h"

class AccCMD
{
public:
	unsigned short senderPort;
	unsigned int id;
	std::vector<Command> accCommands;
	std::string GetCommandData();

private:
};