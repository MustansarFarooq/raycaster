#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include "player.h"
using namespace sf;

// constants
const int screenWidth = 800;
const int screenHeight = 600;
const int mapHeight = 16;
const int mapWidth = 16;
const Color ceilingColor(135,206,235);
const Color floorColor(110,110,110);

// textures to laod in
Image frameBuffer(Vector2u(screenWidth,screenHeight),Color::Black);
Image wallTextureImage;
Texture frameTexture(Vector2u(screenWidth,screenHeight));
Sprite frameSprite(frameTexture);

// camera stuff
const float depth = 16.0f;
Vector2f cameraPlane(0.66f, 0.f);

// refer player.h and the player class
Player player(8.5f, 8.5f);

// stuff we need for delta time
float deltaTime;
Clock deltaClock;

// text for displaying player angle
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
    wallTextureImage.loadFromFile("textures/redbrick.png");
    wallTextureImage.flipVertically(); // 0,0 in an sfml image starts a the top left - we would rather bottom left
    const int textureWidth = wallTextureImage.getSize().x;
    const int textureHeight = wallTextureImage.getSize().y;

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
        for (unsigned y = 0; y < screenHeight; y++)
            {
                Color col = (y<screenHeight/2) ? ceilingColor :floorColor;
                for(unsigned x = 0; x < screenWidth; x++)
                {
                    frameBuffer.setPixel(Vector2u(x,y),col);
                }
            }

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
            bool sideThatRayHit = false; // x side is false, y side is true ig

            float distanceToWall = 0.0f;
            while (!hitWall && distanceToWall < depth)
            {
                if (rayLength.x < rayLength.y)
                {
                    currentTile.x += step.x;
                    distanceToWall = rayLength.x;
                    rayLength.x += unitStepSize.x;
                    sideThatRayHit = false;
                }
                else
                {
                    currentTile.y += step.y;
                    distanceToWall = rayLength.y;
                    rayLength.y += unitStepSize.y;
                    sideThatRayHit = true;
                }

                if (currentTile.x >= 0 && currentTile.x <= mapWidth && currentTile.y >= 0 && currentTile.y <= mapHeight)
                {
                    if (map[int(currentTile.y) * mapWidth + int(currentTile.x)] == 1)
                    {
                        hitWall = true;
                    }
                }
            }




            




            float wallX; // exact place the ray hit the wall ig
            if (!sideThatRayHit)
                wallX = player.position.y + distanceToWall * rayDirection.y;
            else
                wallX = player.position.x + distanceToWall * rayDirection.x;
            wallX -= floor(wallX); // we only want the decimmal part

            int texX = int(wallX * textureWidth); // translates that hit point into where on that texture
            if (!sideThatRayHit && rayDirection.x > 0)
                texX = textureWidth - texX - 1;
            if (sideThatRayHit && rayDirection.y < 0)
                texX = textureWidth - texX - 1;

            int lineHeight = screenHeight / distanceToWall;
            int drawStart = std::max(0, -lineHeight / 2 + screenHeight / 2);
            int drawEnd = std::min(screenHeight - 1, lineHeight / 2 + screenHeight / 2);

            for (int y = drawStart; y < drawEnd; y++)
            {
                // Map screen Y to texture Y
                int d = (y * 256 - screenHeight * 128 + lineHeight * 128);
                int texY = ((d * textureHeight) / lineHeight) / 256;

                sf::Color color = wallTextureImage.getPixel(Vector2u(texX,texY));

                if (sideThatRayHit)
                { // Darken Y-sides for depth cue
                    color.r /= 2;
                    color.g /= 2;
                    color.b /= 2;
                }
                frameBuffer.setPixel(Vector2u(x,y),color);
            }
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
        frameTexture.update(frameBuffer);
        window.draw(frameSprite); //how does my dogshit chatgpt code work
        window.draw(tFPS);
        window.display();
    }
}
