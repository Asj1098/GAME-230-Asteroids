#pragma once
#include "GameObject.h"

class Ship : GameObject
{
public:
	sf::CircleShape shape;
	sf::Vector2f inititalPosition;
	int lives = 3;
	float orientationAngle = -90.0f;
	float rotationSpeed = 150.0f;
	float maxSpeed = 600.0f;
	float radius = 40.0f;

	Ship(float _radius, sf::Vector2f _position, sf::Vector2u _window_size)
	{
		shape.setRadius(_radius);
		shape.setPosition(_position);
		inititalPosition = _position;
		shape.setOrigin(_radius, _radius);
		shape.setRotation(orientationAngle);
		speed = 0;
		direction = sf::Vector2f(0.0f, 1.0f);
		bounds = _window_size;
		radius = _radius;
		type = GameObjectType::Ship;
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
		if (t == GameObjectType::Asteroid)
			lives--;
	}

	void reset()
	{
		shape.setPosition(inititalPosition);
		orientationAngle = -90.0f;
		shape.setRotation(orientationAngle);
		lives = 3;
	}
	void rotateShip(int dir, float delta_time)
	{
		if (dir != 0)
		{
			float rotAngle = dir * rotationSpeed * delta_time;
			//shape.rotate(rotAngle);
			orientationAngle += rotAngle;
			shape.setRotation(orientationAngle);
		}
	}
	void UpdateVelocity(float magnitude, float delta_time)
	{
		direction = sf::Vector2f(cos(3.1426 * orientationAngle / 180), sin(3.1426 * orientationAngle / 180));
		speed += (magnitude * delta_time);
		if (speed < 0)
			speed = 0;
		if (speed > maxSpeed)
			speed = maxSpeed;
	}
	sf::Vector2f getDirection()
	{
		return direction;
	}
	float getAngle()
	{
		return orientationAngle;
	}
};
