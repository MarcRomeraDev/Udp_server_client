#include "AccCMD.h"

void AccCMD::SetCommands(Command command)
{
	accCommands.push_back(command);
}

std::string AccCMD::GetCommandData()
{
	std::string message = "";

	for (const auto& command : accCommands)
	{
		message += command.data + "<";
	}

	accCommands.clear();

	return message;
}