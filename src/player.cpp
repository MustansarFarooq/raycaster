#include "player.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

Player::Player(float x, float y)
{
    position = {x, y};
}

void Player::cameraControls(float deltaTime, sf::Vector2f &cameraPlane)
{

    // counterclockwise rotation matrix:

    // $$R = \begin{bmatrix}\ \cos{\theta}\ -\sin{\theta}\\ \sin{\theta} \ \cos{\theta} \\\end{bmatrix}$$

    // and then to do it clockwise, make theta negative?
    // vector multiplication is weird. i jus got this code from chatgpt but yh fr
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        float cosTheta = cos(rotationSpeed * deltaTime);
        float sinTheta = sin(rotationSpeed * deltaTime);
        float oldDirX = direction.x;

        direction.x = direction.x * cosTheta - direction.y * sinTheta;
        direction.y = oldDirX * sinTheta + direction.y * cosTheta;

        float oldPlaneX = cameraPlane.x;
        cameraPlane.x = cameraPlane.x * cosTheta - cameraPlane.y * sinTheta;
        cameraPlane.y = oldPlaneX * sinTheta + cameraPlane.y * cosTheta;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        float cosTheta = cos(-rotationSpeed * deltaTime);
        float sinTheta = sin(-rotationSpeed * deltaTime);
        float oldDirX = direction.x;

        direction.x = direction.x * cosTheta - direction.y * sinTheta;
        direction.y = oldDirX * sinTheta + direction.y * cosTheta;

        float oldPlaneX = cameraPlane.x;
        cameraPlane.x = cameraPlane.x * cosTheta - cameraPlane.y * sinTheta;
        cameraPlane.y = oldPlaneX * sinTheta + cameraPlane.y * cosTheta;
    }
}

void Player::movement(float deltaTime)
{

    velocity = {0.f, 0.f};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        velocity = direction * movementSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        velocity = -direction * movementSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        sf::Vector2f leftStrafe = sf::Vector2f(-direction.y, direction.x);
        velocity = leftStrafe * movementSpeed * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        sf::Vector2f rightStrafe = sf::Vector2f(direction.y, -direction.x);
        velocity = rightStrafe * movementSpeed * deltaTime;
    }
    projectedPosition = position + velocity;
}

sf::Vector2f Player::getProjectedPosition()
{
    return projectedPosition;
}
sf::Vector2f Player::getDirection()
{
    return direction;
}
