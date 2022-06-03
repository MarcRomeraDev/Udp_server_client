#include "Command.h"

Command::Command() : data(""), confirmationHeader(Header::COUNT), senderPort(0)
{

}

Command::Command(std::string _data, Header _confirmationHeader, unsigned short port, unsigned int _id) : data(_data), confirmationHeader(_confirmationHeader), senderPort(port), id(_id)
{

}