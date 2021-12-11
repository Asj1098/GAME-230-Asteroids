#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>

enum class GameObjectType { Ship, Asteroid, Laser };

class GameObject
{
public:
	sf::Vector2f direction;
	float speed;
	sf::Vector2u bounds;
	GameObjectType type;
	bool isVisible = true;

	virtual void update(float delta_time) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual sf::Vector2f getCenter() = 0;
	virtual sf::Vector2f getPosition() = 0;
	virtual sf::Vector2f getPositionR() = 0;
	virtual bool checkCollisionWith(GameObject* obj) = 0;
	virtual sf::FloatRect getGlobalBounds() = 0;
	virtual void handleCollision(GameObjectType t) = 0;
};

