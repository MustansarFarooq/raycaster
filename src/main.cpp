#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include "player.h"
using namespace sf;

//constants
const int screenWidth = 800;
const int screenHeight = 600;
const int mapHeight = 16;
const int mapWidth = 16;

//camera stuff
const float depth = 16.0f;
Vector2f cameraPlane(0.66f, 0.f);

//refer player.h and the player class
Player player(8.5f, 8.5f);

//stuff we need for delta time
float deltaTime;
Clock deltaClock;

//text for displaying player angle
const Font font("font.ttf");
Text playerAngle(font, std::to_string(atan2(player.getDirection().y, player.getDirection().x)), 20);

const int map[mapHeight * mapWidth] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

int main()
{
    RenderWindow window(VideoMode({screenWidth, screenHeight}), "Raycaster");
    VertexArray line(PrimitiveType::Lines, 2);
    float fpsTimer = 0;
    std::ostringstream sFPS; // string buffer for doing stuff with numbers and strings and idk stack overflow got this for me
    std::ostringstream sPAngle;

    Text tFPS(font, '0', 20);
    tFPS.setPosition({10, 10});
    while (window.isOpen())
    {
        deltaTime = deltaClock.restart().asSeconds();
        fpsTimer += deltaTime;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
        }

        player.cameraControls(deltaTime, cameraPlane);

        player.movement(deltaTime);

        if (map[(int)player.getProjectedPosition().y * mapWidth + (int)player.getProjectedPosition().x] != 1)
        {
            player.position = player.getProjectedPosition();
        }

        window.clear();

        for (int x = 0; x < screenWidth; x++)
        {

            float cameraX = 2 * float(x) / float(screenWidth) - 1; // range from -1 to 1? idk
            Vector2f rayDir = player.getDirection() + cameraPlane * cameraX;

            // $$\hat{V} = \begin{bmatrix}\sin{\theta}\\\cos{\theta}\\\end{bmatrix}$$

            Vector2f rayDirection = rayDir;
            Vector2f rayStart = player.position;

            // we get how much we travel per y for one step in x-axis and vice versa [DDA]

            //$$One\space step\space in\space X\space axis\space = \sqrt{1+\left(\frac{rayDir.y}{rayDir.x}\right)^2}$$     
            
            

            //$$One\space step\space in\space Y\space axis\space = \sqrt{1+\left(\frac{rayDir.x}{rayDir.y}\right)^2}$$
            

            Vector2f unitStepSize(sqrt(1 + (rayDirection.y / rayDirection.x) * (rayDirection.y / rayDirection.x)), sqrt(1 + (rayDirection.x / rayDirection.y) * (rayDirection.x / rayDirection.y)));

            Vector2i currentTile(int(rayStart.x), int(rayStart.y));
            Vector2f rayLength; // the x value is the ray lenght when we add the step.x, and the y value is when we add step.y
            Vector2f step;      // negative or positive depending on direction

            if (rayDirection.x < 0)
            {
                step.x = -1;
                rayLength.x = (rayStart.x - currentTile.x) * unitStepSize.x;
            }
            else
            {
                step.x = 1;
                rayLength.x = ((currentTile.x + 1) - rayStart.x) * unitStepSize.x;
            }

            if (rayDirection.y < 0)
            {
                step.y = -1;
                rayLength.y = (rayStart.y - currentTile.y) * unitStepSize.y;
            }
            else
            {
                step.y = 1;
                rayLength.y = ((currentTile.y + 1) - rayStart.y) * unitStepSize.y;
            }
            bool hitWall = false;
            float distanceToWall = 0.0f;
            while (!hitWall && distanceToWall < depth)
            {
                if (rayLength.x < rayLength.y)
                {
                    currentTile.x += step.x;
                    distanceToWall = rayLength.x;
                    rayLength.x += unitStepSize.x;
                }
                else
                {
                    currentTile.y += step.y;
                    distanceToWall = rayLength.y;
                    rayLength.y += unitStepSize.y;
                }

                if (currentTile.x >= 0 && currentTile.x <= mapWidth && currentTile.y >= 0 && currentTile.y <= mapHeight)
                {
                    if (map[int(currentTile.y) * mapWidth + int(currentTile.x)] == 1)
                    {
                        hitWall = true;
                    }
                }
            }

            float ceiling = (float)(screenHeight / 2.0f) - (screenHeight / (float)distanceToWall);
            float floor = screenHeight - ceiling;
            uint8_t shade = static_cast<uint8_t>(255 - std::min(distanceToWall * 16, 255.0f));
            line[0].color = Color(shade, 0, 0);
            line[1].color = Color(shade, 0, 0);
            line[0].position = Vector2f(x, ceiling);
            line[1].position = Vector2f(x, floor);
            window.draw(line);
        }

        // updating the fps once per second
        if (fpsTimer >= 1)
        {
            float fps = 1 / deltaTime;
            fps = trunc(fps);
            sFPS.str("");
            sFPS << fps;
            tFPS.setString(sFPS.str());
            fpsTimer = 0;
        }
        window.draw(tFPS);

        window.display();
    }
}
