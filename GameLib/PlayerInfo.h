#pragma once
#include <SFML\Graphics.hpp>
#include <chrono>

class PlayerInfo
{
public:
	//Client info
	unsigned short port;
	std::string ip;
	std::string name;
	// Timestamp del ultimo mensage
	std::chrono::system_clock::time_point timeStamp;
	uint32_t serverSalt; // Salt generated from the server
	uint32_t clientSalt; // The result of the AND of both Salts
	uint32_t saltTag;
	int challengeSolution;
	int attemptCount;


	//Player ingame info
	int playerNum;
	sf::Vector2i position;
	float shotCooldown;
	float movementSpeed;
	float speedMultiplier;

	PlayerInfo();
	~PlayerInfo();
};