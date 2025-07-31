#include "player.h"
#include <SFML/Graphics.hpp>
#include <cmath>

Player::Player(float x, float y)
{
    position = {x,y};
}

void Player::cameraControls(float deltaTime, sf::Vector2f& cameraPlane)
{

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            float cosTheta = cos(rotationSpeed * deltaTime);
            float sinTheta = sin(rotationSpeed * deltaTime);
            float oldDirX = direction.x;

            direction.x = direction.x * cosTheta - direction.y * sinTheta;
            direction.y = oldDirX * sinTheta + direction.y * cosTheta;

            float oldPlaneX = cameraPlane.x;
            cameraPlane.x = cameraPlane.x*cosTheta - cameraPlane.y * sinTheta;
            cameraPlane.y = oldPlaneX*sinTheta + cameraPlane.y *cosTheta;

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            float cosTheta = cos(-rotationSpeed  * deltaTime);
            float sinTheta = sin(-rotationSpeed  * deltaTime);
            float oldDirX = direction.x;

            direction.x = direction.x * cosTheta - direction.y * sinTheta;
            direction.y = oldDirX * sinTheta + direction.y * cosTheta;

            float oldPlaneX = cameraPlane.x;
            cameraPlane.x = cameraPlane.x*cosTheta - cameraPlane.y * sinTheta;
            cameraPlane.y = oldPlaneX*sinTheta + cameraPlane.y *cosTheta;

        }
}

sf::Vector2f Player::movement(float deltaTime)
{
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            velocity =direction*movementSpeed*deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            velocity = -direction * movementSpeed * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            sf::Vector2f leftStrafe = sf::Vector2f(-direction.y, direction.x);
            velocity = leftStrafe *  movementSpeed * deltaTime;

        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            sf::Vector2f rightStrafe = sf::Vector2f(direction.y, -direction.x);
            velocity = rightStrafe * movementSpeed * deltaTime;
        }
        sf::Vector2f projectedPosition = position + velocity;
        return projectedPosition;
        
        
}

sf::Vector2f Player::getPosition() const
{
    return position;
}
sf::Vector2f Player::getDirection() const
{
    return direction;
}