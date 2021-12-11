#pragma once
#include "GameObject.h"

class Laser : GameObject
{
public:
	sf::CircleShape shape;
	float radius;
	float timeToDeactivate = 0;
	sf::Vector2f inititalPosition;

	Laser(float _radius, sf::Vector2u _window_size)
	{
		shape.setRadius(_radius);
		shape.setFillColor(sf::Color::Red);
		shape.setOrigin(_radius, _radius);
		bounds = _window_size;
		radius = _radius;
		type = GameObjectType::Laser;
		isVisible = false;
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
		timeToDeactivate -= delta_time;

		if (timeToDeactivate <= 0)
			isVisible = false;
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
	virtual sf::Vector2f getPositionR()
	{
		return sf::Vector2f(shape.getPosition().x + (radius * 2), shape.getPosition().y + (radius * 2));
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
		if (t == GameObjectType::Asteroid)
		{
			isVisible = false;
		}
	}

	void Fire(sf::Vector2f _position, float _speed, float _timeToDeactivate, sf::Vector2f _direction)
	{
		_position.x -= radius;
		_position.y -= radius;
		shape.setPosition(_position);
		speed = _speed;
		direction = _direction;
		timeToDeactivate = _timeToDeactivate;
		isVisible = true;
	}
	void Fire(sf::Vector2f _position, float _speed, float _timeToDeactivate, float angle)
	{
		shape.setPosition(_position);
		speed = _speed;
		direction = sf::Vector2f(cos(3.1426 * angle / 180), sin(3.1426 * angle / 180));
		timeToDeactivate = _timeToDeactivate;
		isVisible = true;
	}
	void Reset()
	{
		isVisible = false;
	}
	bool isAvailable()
	{
		return !isVisible;
	}
};

