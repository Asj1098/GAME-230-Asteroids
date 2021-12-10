#pragma once
#include "GameObject.h"

class Asteroid : GameObject
{
public:
	sf::CircleShape shape;
	float radius;
	sf::Vector2f inititalPosition;
	int size = 3;

	Asteroid(float _radius, sf::Vector2f _position, float _speed, sf::Vector2f _direction, sf::Vector2u _window_size, int _size)
	{
		shape.setRadius(_radius);
		shape.setPosition(_position);
		shape.setOrigin(_radius, _radius);
		inititalPosition = _position;
		speed = _speed;
		direction = _direction;
		bounds = _window_size;
		radius = _radius;
		type = GameObjectType::Asteroid;
		size = _size;

	}

	virtual void update(float delta_time)
	{
		sf::Vector2f position = shape.getPosition();
		position.x += direction.x * speed * delta_time;
		position.y += direction.y * speed * delta_time;
		
		if (position.x + 2 * radius < 0)
			position.x = bounds.x - 5;

		if (position.y + 2 * radius < 0)
			position.y = bounds.y - 5;

		if (position.x > bounds.x)
			position.x = 5;

		if (position.y > bounds.y)
			position.y = 5;

		shape.setPosition(position);
	}
	virtual void draw(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	virtual sf::Vector2f getCenter()
	{
		return sf::Vector2f(shape.getPosition().x + shape.getRadius(), shape.getPosition().y + shape.getRadius());
	}
	virtual sf::Vector2f getPosition()
	{
		return shape.getPosition();
	}
	virtual bool checkCollisionWith(GameObject* obj)
	{
		return(shape.getGlobalBounds().intersects(obj->getGlobalBounds()));
	}
	virtual sf::FloatRect getGlobalBounds()
	{
		return shape.getGlobalBounds();
	}
	virtual void handleCollision(GameObjectType t)
	{
		if (t == GameObjectType::Ship)
			isVisible = false;
		else if (t == GameObjectType::Asteroid)
		{
			direction.x = -direction.x;
			direction.y = -direction.y;
		}
		else if (t == GameObjectType::Laser)
		{
			isVisible = false;
		}
	}
};


