#pragma once
#include <SFML\Graphics.hpp>

class PlayerInfo
{
public:
	std::string name;
	sf::Vector2i position;
	int lives;
	PlayerInfo();
	~PlayerInfo();
};