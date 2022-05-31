#include <iostream>
#include <fcntl.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include "Client.h"
#define SIZE 10.f
///TAMAÑO EN PX DE LA VENTANA
#define W_WINDOW_PX 800
#define H_WINDOW_PX 600

///TAMAÑ˜O EN CELDAS DE LA VENTANA
#define W_WINDOW_TITLE 80
#define H_WINDOW_TITLE 60

///TAMAÑO EN CELDAS DE UNA ZONA DE MAPA
#define W_WINDOW_TITLE_PART 40
#define H_WINDOW_TITLE_PART 30

///NUMERO DE ENEMIGOS Y DE OBSTACULOS QUE TIENEN QUE GENERARSE POR ZONA
#define NUM_ENEMIES 5
#define NUM_OBSTACLES_2x2 10

#define MAX_MENSAJES 30
void DrawDungeon()
{
	sf::RenderWindow _window(sf::VideoMode(800, 600), "Ventanita");
	sf::RectangleShape shape(sf::Vector2f(SIZE, SIZE));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(2.f);

	while (_window.isOpen())
	{
		sf::Event event;
		bool playerMoved = false;
		while (_window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				_window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
				{
					//Desconexion
					_window.close();
				}
				if (event.key.code == sf::Keyboard::Left)
				{
					std::cout << "LEFT\n";
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					std::cout << "UP\n";
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					std::cout << "RIGTH\n";
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					std::cout << "DOWN\n";
				}
				break;
			}
		}
		_window.clear();
		for (int i = 0; i < W_WINDOW_TITLE; i++)
		{
			for (int j = 0; j < H_WINDOW_TITLE; j++)
			{

				shape.setFillColor(sf::Color(90, 90, 90, 255));


				shape.setPosition(sf::Vector2f(i * SIZE, j * SIZE));
				_window.draw(shape);
			}
		}

		sf::Vector2f position;
		position.x = 0; position.y = 0;
		shape.setFillColor(sf::Color::Blue);
		shape.setFillColor(sf::Color(0, 0, 255, 255));
		shape.setPosition(sf::Vector2f(position.x * SIZE, position.y * SIZE));
		_window.draw(shape);

		position.x = W_WINDOW_TITLE - 1; position.y = H_WINDOW_TITLE - 1;
		shape.setFillColor(sf::Color::Green);
		shape.setFillColor(sf::Color(255, 255, 0, 255));
		shape.setPosition(sf::Vector2f(position.x * SIZE, position.y * SIZE));
		_window.draw(shape);

		_window.display();
	}

}
int main()
{
	
	srand(static_cast<unsigned>(time(nullptr)));
	Client client;
	// Entras / Creas una partida
	DrawDungeon();
}

//std::vector<std::string> aMensajes;
//
//sf::Vector2i screenDimensions(800, 600);
//
//sf::RenderWindow window;
//window.create(sf::VideoMode(screenDimensions.x, screenDimensions.y), "Chat");
//
//sf::Font font;
//if (!font.loadFromFile("courbd.ttf"))
//{
//	std::cout << "Can't load the font file" << std::endl;
//}
//
//sf::String mensaje = ">";
//
//sf::Text chattingText(mensaje, font, 14);
//chattingText.setFillColor(sf::Color(0, 160, 0));
//chattingText.setStyle(sf::Text::Bold);
//
//sf::Text text(mensaje, font, 14);
//text.setFillColor(sf::Color(0, 160, 0));
//text.setStyle(sf::Text::Bold);
//text.setPosition(0, 560);
//
//sf::RectangleShape separator(sf::Vector2f(800, 5));
//separator.setFillColor(sf::Color(200, 200, 200, 255));
//separator.setPosition(0, 550);
//
//
//while (window.isOpen())
//{
//	sf::Event evento;
//	while (window.pollEvent(evento))
//	{
//		switch (evento.type)
//		{
//		case sf::Event::Closed:
//			// Send message de desconexion
//			window.close();
//			break;
//		case sf::Event::KeyPressed:
//			//client.Test();
//			if (evento.key.code == sf::Keyboard::Escape)
//				window.close();
//			else if (evento.key.code == sf::Keyboard::Return)
//			{
//				aMensajes.push_back(mensaje);
//				mensaje.erase(0, 1);
//				client.SendMessage(mensaje); //-->Client sends message
//				if (aMensajes.size() > 25)
//				{
//					aMensajes.erase(aMensajes.begin(), aMensajes.begin() + 1);
//				}
//				mensaje = ">";
//
//			}
//			break;
//		case sf::Event::TextEntered:
//			if (evento.text.unicode >= 32 && evento.text.unicode <= 126)
//				mensaje += (char)evento.text.unicode;
//			else if (evento.text.unicode == 8 && mensaje.getSize() > 0)
//				mensaje.erase(mensaje.getSize() - 1, mensaje.getSize());
//			break;
//		}
//	}
//	window.draw(separator);
//	for (size_t i = 0; i < aMensajes.size(); i++)
//	{
//		std::string chatting = aMensajes[i];
//		chattingText.setPosition(sf::Vector2f(0, 20 * i));
//		chattingText.setString(chatting);
//		window.draw(chattingText);
//	}
//	std::string mensaje_ = mensaje + "_";
//	text.setString(mensaje_);
//	window.draw(text);
//
//	window.display();
//	window.clear();
//}