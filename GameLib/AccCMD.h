#pragma once
#include <vector>
#include "Command.h"

class AccCMD
{
public:
	unsigned short senderPort;
	unsigned int id;
	void SetCommands(Command command);
	std::string GetCommandData();

private:
	std::vector<Command> accCommands;
};