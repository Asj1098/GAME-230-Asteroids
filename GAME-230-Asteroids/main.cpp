#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <vector>
#include <iostream>

#include "GameObject.h"
#include "Asteroid.h"
#include "Laser.h"
#include "Ship.h"

enum class GameState { Menu, InGame, GameOver };

std::vector<GameObject*> objects;
const float BUCKET_WIDTH = 400;
const float BUCKET_HEIGHT = 400;
const int COLUMNS = 3;
const int ROWS = 3;
std::vector<GameObject*> grid[COLUMNS][ROWS];
std::vector<Laser*> lasers;
int score = 0;
sf::Vector2u window_size;

//asteroids
float asteroidSpeed = 50.0f;
float asteroidRadius = 80.0f;
int largeAsteroidCount = 4;
sf::Texture asteroidTextures[3];

//sounds
sf::Sound asteroid_sound;
sf::Sound hit_sound;

sf::Vector2i getBucket(sf::Vector2f pos)
{
    int col = int(pos.x / BUCKET_WIDTH);
    if (col < 0)
        col = 0;
    else if (col >= COLUMNS)
        col = COLUMNS - 1;

    int row = int(pos.y / BUCKET_HEIGHT);
    if (row < 0)
        row = 0;
    else if (row >= ROWS)
        row = ROWS - 1;

    return sf::Vector2i(col, row);
}

void bucket_add(sf::Vector2i b,GameObject* obj)
{
    //std::vector<GameObject*>& v = grid[b.x, b.y];
    //v.push_back(obj);
    grid[b.x][b.y].push_back(obj);
}

void bucket_remove(sf::Vector2i b, GameObject* obj)
{
    //std::vector<GameObject*>& v = grid[b.x, b.y];

    for (int i = 0; i < grid[b.x][b.y].size(); ++i)
    {
        if (grid[b.x][b.y][i] == obj)
        {
            grid[b.x][b.y].erase(grid[b.x][b.y].begin() + i);
            break;
        }
    }
}

int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

int min(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

void detect_collisions(GameObject* obj, sf::Vector2i b)
{
    int left = max(b.x - 1, 0);
    int right = min(b.x + 1, COLUMNS - 1);
    int top = max(b.y - 1, 0);
    int bot = min(b.y + 1, ROWS - 1);

    for (int bx = left; bx <= right; ++bx)
    {
        for (int by = top; by <= bot; ++by)
        {
            std::vector<GameObject*>& v = grid[b.x][b.y];
            for (GameObject* o : v)
            {
                if (o != obj)
                    obj->checkCollisionWith(o);
            }
        }
    }
}
void SpawnAsteroids(float radius, sf::Vector2f pos, float speed, sf::Vector2f direction, sf::Vector2u window_size, int asteroidSize);

void bucket_collision_check(int x, int y)
{
    for (int i = 0; i < grid[x][y].size(); i++)
    {
        for (int j = i + 1; j < grid[x][y].size(); j++)
        {
            if (grid[x][y][i]->checkCollisionWith(grid[x][y][j]))
            {
                grid[x][y][i]->handleCollision(grid[x][y][j]->type);
                grid[x][y][j]->handleCollision(grid[x][y][i]->type);
                if ((grid[x][y][i]->type == GameObjectType::Ship && grid[x][y][j]->type == GameObjectType::Asteroid) || (grid[x][y][j]->type == GameObjectType::Ship && grid[x][y][i]->type == GameObjectType::Asteroid))
                    hit_sound.play();
                if ((grid[x][y][i]->type == GameObjectType::Asteroid && grid[x][y][j]->type == GameObjectType::Laser) || (grid[x][y][i]->type == GameObjectType::Laser && grid[x][y][j]->type == GameObjectType::Asteroid))
                {
                    score++;
                    asteroid_sound.play();
                    Asteroid* a = nullptr;

                    if (grid[x][y][i]->type == GameObjectType::Asteroid)
                        a = (Asteroid*)grid[x][y][i];
                    if (grid[x][y][i]->type == GameObjectType::Laser)
                        a = (Asteroid*)grid[x][y][j];

                    if (a == nullptr)
                        continue;

                    float _r = asteroidRadius;
                    if (a->size == 3)
                        _r = _r *0.75;
                    if (a->size == 2)
                        _r = _r / 3;

                    if (a->size > 1)
                    {
                        float xDir = 1, yDir = 1;
                        if (rand() % 2 == 0)
                            xDir = -1;
                        if (rand() % 2 == 0)
                            yDir = -1;
                        if (rand() % 2 == 0)
                        {
                            if (rand() % 2 == 0)
                                xDir = 0;
                            else
                                yDir = 0;
                        }
                        sf::Vector2f spawnPosition = a->getPosition();
                        SpawnAsteroids(_r, spawnPosition, asteroidSpeed, sf::Vector2f(xDir, yDir), window_size, a->size-1);
                        spawnPosition.x += a->radius * 2;
                        spawnPosition.y += a->radius * 2;
                        SpawnAsteroids(_r, spawnPosition, asteroidSpeed, sf::Vector2f(-xDir, -yDir), window_size, a->size-1);
                    }

                }
            }
        }
    }
    grid[x][y].clear();
}


void SpawnAsteroids(float radius, sf::Vector2f pos, float speed, sf::Vector2f direction, sf::Vector2u window_size, int asteroidSize)
{
    Asteroid* a = new Asteroid(radius, pos, speed, direction, window_size, asteroidSize);
    a->shape.setTexture(&asteroidTextures[rand()%3]);
    objects.push_back((GameObject*)a);
}

void RandomSpawnAsteroids(int acount,sf::Vector2u window_size)
{

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if ((i == 1 && j == 1) || (acount == 0))
                continue;
            float xPos = (BUCKET_HEIGHT * i) + (rand() % 200);
            float yPos = (BUCKET_WIDTH * j) + (rand() % 200);

            float xDir = 1, yDir = 1;
            if (rand() % 2 == 0)
                xDir = -1;
            if (rand() % 2 == 0)
                yDir = -1;
            if (rand() % 2 == 0)
            {
                if (rand() % 2 == 0)
                    xDir = 0;
                else
                    yDir = 0;
            }
            SpawnAsteroids(asteroidRadius, sf::Vector2f(xPos, yPos), asteroidSpeed, sf::Vector2f(xDir, yDir), window_size, 3);
            acount--;
        }
    }
}

//Lasers

int main()
{
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(1200, 1200), "Asteroids");
    window_size = window.getSize();

    //set up V-Sync
    window.setVerticalSyncEnabled(true);

    //Clock to count the time taken for a frame
    sf::Clock clock;

    //CORE
    float ship_radius = 40.0f;
    //Ship ship(ship_radius, sf::Vector2f(window.getSize().x/2 - ship_radius, window.getSize().y/2 - ship_radius), window.getSize());
    Ship* ship = new Ship(ship_radius, sf::Vector2f(window.getSize().x / 2 - ship_radius, window.getSize().y / 2 - ship_radius), window.getSize());


    //Load Font
    sf::Font font;
    if (!font.loadFromFile("Resources/Roboto-Regular.ttf"))
    {
        std::cout << "Font not loaded\n";
        return EXIT_FAILURE;
    }

    //Text
    sf::Text game_text;
    game_text.setFont(font);
    game_text.setCharacterSize(90.0f);
    game_text.setString("Asteroids\n\nPress Space to Play\nPress Q to Exit");
    game_text.setFillColor(sf::Color::Magenta);
    game_text.setPosition(sf::Vector2f(window.getSize().x * 0.2f, window.getSize().y * 0.2f));

    sf::Text score_text;
    score_text.setFont(font);
    score_text.setString("Score: 0");
    score_text.setCharacterSize(70.0f);
    score_text.setFillColor(sf::Color::Yellow);
    score_text.setPosition(sf::Vector2f(window.getSize().x * 0.75f, 50.0f));

    sf::Text level_text;
    level_text.setFont(font);
    level_text.setString("Level: 1");
    level_text.setCharacterSize(70.0f);
    level_text.setFillColor(sf::Color::Red);
    level_text.setPosition(sf::Vector2f(window.getSize().x * 0.4f, 50.0f));

    sf::Text lives_text;
    lives_text.setFont(font);
    lives_text.setString("Lives: 3");
    lives_text.setCharacterSize(70.0f);
    lives_text.setFillColor(sf::Color::Green);
    lives_text.setPosition(sf::Vector2f(100.0f, 50.0f));

    //Load Textures
    //sf::Texture asteroidTextures[3];
    if (!asteroidTextures[0].loadFromFile("Resources/meteorBrown_big1.png"))
        return EXIT_FAILURE;
    if (!asteroidTextures[1].loadFromFile("Resources/meteorBrown_big3.png"))
        return EXIT_FAILURE;
    if (!asteroidTextures[2].loadFromFile("Resources/meteorBrown_big4.png"))
        return EXIT_FAILURE;

    sf::Texture shipTexture;
    if (!shipTexture.loadFromFile("Resources/playerShip3_red.png"))
        return EXIT_FAILURE;

    sf::Texture shipThrustTexture;
    if (!shipThrustTexture.loadFromFile("Resources/playerShip3_red_thruster.png"))
        return EXIT_FAILURE;

    //Apply Textures
    ship->shape.setTexture(&shipTexture);

    //Load Sounds
    sf::SoundBuffer laser_buffer;
    if (!laser_buffer.loadFromFile("Resources/Hit_03.wav"))
        return EXIT_FAILURE;
    sf::Sound laser_sound;
    laser_sound.setBuffer(laser_buffer);


    sf::SoundBuffer asteroid_buffer;
    if (!asteroid_buffer.loadFromFile("Resources/Explosion_03.wav"))
        return EXIT_FAILURE;

    asteroid_sound.setBuffer(asteroid_buffer);

    sf::SoundBuffer hit_buffer;
    if (!hit_buffer.loadFromFile("Resources/Hit_02.wav"))
        return EXIT_FAILURE;

    hit_sound.setBuffer(hit_buffer);

    sf::SoundBuffer win_buffer;
    if (!win_buffer.loadFromFile("Resources/Jingle_Achievement_00.wav"))
        return EXIT_FAILURE;

    sf::Sound win_sound;
    win_sound.setBuffer(win_buffer);

    sf::SoundBuffer lose_buffer;
    if (!lose_buffer.loadFromFile("Resources/Jingle_Lose_00.wav"))
        return EXIT_FAILURE;

    sf::Sound lose_sound;
    lose_sound.setBuffer(lose_buffer);

    sf::SoundBuffer thrust_buffer;
    if (!thrust_buffer.loadFromFile("Resources/rocket_launch.wav"))
        return EXIT_FAILURE;

    sf::Sound thrust_sound;
    thrust_sound.setBuffer(thrust_buffer);
    thrust_sound.setLoop(true);

    sf::Music music_buffer;
    //Credits: opengameart.org/content/space-boss-battle-theme
    if (!music_buffer.openFromFile("Resources/OrbitalColossus.wav"))
        return EXIT_FAILURE;
    music_buffer.setVolume(50.0f);

    //Lasers
    int maxLasers = 5;
    float laserRadius = 10.0f;
    float laserSpeed = 800.0f;
    for (int i = 0; i < maxLasers; i++)
    {
        Laser* l = new Laser(laserRadius, window.getSize());
        lasers.push_back(l);
    }

    //Game State
    GameState gameState = GameState::Menu;
    bool spacePressed = false;
    bool playthrust = false;
    int level = 1;
    //Restart
RESTART:
    gameState = GameState::Menu;
    game_text.setString("Asteroids\n\nPress Space to Play\nPress Q to Exit");
    ship->reset();
    level = 1;
    level_text.setString("Level: 1");

    score = 0;
    score_text.setString("0");
    lives_text.setString("Lives: 3");
    asteroidSpeed = 80.0f;
    largeAsteroidCount = 4;
    playthrust = false;

    music_buffer.stop();
    thrust_sound.stop();

LEVELUP:
    spacePressed = false;    
    objects.clear();
    objects.push_back((GameObject*)ship);

    for (int i = 0; i < lasers.size(); i++)
    {
        //lasers[i]->Reset();
        objects.push_back((GameObject*)lasers[i]);
    }

    //Spawn random asteroids
    RandomSpawnAsteroids(largeAsteroidCount, window.getSize());
    
    while (window.isOpen())
    {
        //Update clock
        float delta_time = clock.getElapsedTime().asSeconds();
        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        //Finite state machine for different game states
        if (gameState == GameState::Menu)
        {
            window.draw(game_text);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            {
                gameState = GameState::InGame;
                music_buffer.play();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
            {
                return 1;
            }

            window.display();
            continue;
        }
        else if (gameState == GameState::InGame)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            {
                gameState = GameState::Menu;
                goto RESTART;
            }

            //Inputs
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
                    ship->rotateShip(-1, delta_time);

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
                    ship->rotateShip(1, delta_time);

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    ship->shape.setTexture(&shipThrustTexture);
                    ship->UpdateVelocity(400.0f, delta_time);
                    if (!playthrust)
                    {
                        thrust_sound.play();
                        playthrust = true;
                    }
                }
                else
                {
                    ship->shape.setTexture(&shipTexture);
                    ship->UpdateVelocity(-300.0f, delta_time);
                    if (playthrust)
                    {
                        thrust_sound.stop();
                        playthrust = false;
                    }
                }
                
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)  || sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    if (!spacePressed)
                    {
                        for (int i = 0; i < lasers.size(); i++)
                        {
                            if (lasers[i]->isAvailable())
                            {
                                lasers[i]->Fire(ship->getPosition(), laserSpeed, 2.0f, ship->getAngle());
                                laser_sound.play();
                                break;
                            }
                        }
                    }
                    spacePressed = true;
                }
                else
                    spacePressed = false;
            }

            //buckets
            {
                for (int i = 0; i < objects.size(); i++)
                {
                    if (!objects[i]->isVisible)
                        continue;
                    //sf::Vector2i bp = getBucket(objects[i]->getCenter());
                    sf::Vector2i bp = getBucket(objects[i]->getPosition());
                    sf::Vector2i bpr = getBucket(objects[i]->getPositionR());
                    //bucket_add(bp, objects[i]);

                    //Add to multiple buckets incase object is on edge of two or more buckets
                    for (int k = bp.x; k <= bpr.x; k++)
                    {
                        for (int l = bp.y; l <= bpr.y; l++)
                            bucket_add(sf::Vector2i(k, l), objects[i]);
                    }
                }
                for (int i = 0; i < ROWS; i++)
                {
                    for (int j = 0; j < COLUMNS; j++)
                    {
                        bucket_collision_check(i, j);
                    }
                }
            }

            //Draw & check levelup
            bool levelup = true;
            for (int i = 0; i < objects.size(); i++)
            {
                if (objects[i]->isVisible)
                {
                    objects[i]->draw(window);
                    objects[i]->update(delta_time);
                    if (objects[i]->type == GameObjectType::Asteroid)
                        levelup = false;
                }
            }

            if (levelup)
            {
                levelup = false;
                win_sound.play();
                asteroidSpeed += 80.0f;
                asteroidRadius += 30.0f;
                largeAsteroidCount++;
                level++;
                level_text.setString("Level: " + std::to_string(level));
                if (largeAsteroidCount > 7)
                    largeAsteroidCount = 7;
                goto LEVELUP;
                continue;
            }

            //Game Over
            if (ship->lives < 0)
            {
                gameState = GameState::GameOver;
                music_buffer.stop();
                lose_sound.play();
                continue;
            }

            lives_text.setString("Lives: " + std::to_string(ship->lives));
            score_text.setString("Score: " + std::to_string(score));
            window.draw(score_text);
            window.draw(lives_text);
            window.draw(level_text);
            window.display();

            continue;
        }
        else if (gameState == GameState::GameOver)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
            {
                gameState = GameState::Menu;
                goto RESTART;
            }
            music_buffer.stop();
            thrust_sound.stop();
            game_text.setString("GAME OVER!\n\nPress Enter\nto go to Menu");
            window.draw(game_text);
            window.draw(level_text);
            score_text.setString("Score: " + std::to_string(score));
            window.draw(score_text);
            window.display();
        }
    }

    return 0;
}
