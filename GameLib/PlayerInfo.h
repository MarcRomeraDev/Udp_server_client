#pragma once
#include <SFML\Graphics.hpp>

class PlayerInfo
{
public:
	//Client info
	unsigned short port;
	std::string ip;
	std::string name;
	// Timestamp del ultimo mensage
	unsigned int timeStamp;
	uint32_t serverSalt; // Salt generated from the server
	uint32_t clientSalt; // The result of the AND of both Salts
	int challengeSolution;
	int attemptCount;

	uint32_t saltTag; //<<<<TIENE QUE SER UN BINARIO???>>>>

	//Player ingame info
	sf::Vector2i position;
	float shotCooldown;
	float movementSpeed;
	float speedMultiplier;

	PlayerInfo();
	~PlayerInfo();
};