#pragma once
#include <SFML/Graphics.hpp>

class Player
{   
    sf::Vector2f velocity{0.f,0.f};
    sf::Vector2f projectedPosition;
    const float movementSpeed = 5.f;
    const float rotationSpeed = 1.5f;


public:
    sf::Vector2f direction{0.f,1.f};    
    sf::Vector2f position;
    Player(float x, float y);

    void movement(float deltaTime);
    void cameraControls(float deltaTime, sf::Vector2f& cameraPlane);
    void draw(sf::RenderWindow &window);

    sf::Vector2f getProjectedPosition();
    sf::Vector2f getDirection();
};