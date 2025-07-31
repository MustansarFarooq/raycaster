#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include "player.h"
using namespace sf;
using namespace std;

const Font font("font.ttf");
const int screenWidth = 800;
const int screenHeight = 600;
const int mapHeight = 16;
const int mapWidth = 16;
const float playerSpeed = 5.0f;
const float rotationSpeed = 1.5f;

const float depth = 16.0f;
Vector2f playerPosition(8.5f, 8.5f);
Vector2f playerDirection(0.f, 1.f);
Vector2f cameraPlane(0.66f, 0.f);
Vector2f playerVelocity(0.f, 0.f);

//Player player(8.5f,8.5f);



float deltaTime;
Clock deltaClock;
Text playerAngle(font, to_string(atan2(playerDirection.y, playerDirection.x)), 20);

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

int main() {
    RenderWindow window(VideoMode({screenWidth, screenHeight}), "Raycaster");
    VertexArray line(PrimitiveType::Lines, 2);
    float fpsTimer = 0;
    ostringstream sFPS; // string buffer for doing stuff with numbers and strings and idk stack overflow got this for me
    ostringstream sPAngle;

    Text tFPS(font, '0', 20);
    tFPS.setPosition({10, 10});
    while (window.isOpen()) {
        playerVelocity = Vector2f(0.f, 0.f);
        deltaTime = deltaClock.restart().asSeconds();
        fpsTimer += deltaTime;

        while (const optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }



        //counterclockwise rotation matrix:

        // $$R = \begin{bmatrix}\ \cos{\theta}\ -\sin{\theta}\\ \sin{\theta} \ \cos{\theta} \\\end{bmatrix}$$

        // and then to do it clockwise, make theta negative?
        // vector multiplication is weird. i jus got this code from chatgpt but yh fr 



        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
            float cosTheta = cos(rotationSpeed * deltaTime);
            float sinTheta = sin(rotationSpeed * deltaTime);
            float oldDirX = playerDirection.x;

            playerDirection.x = playerDirection.x * cosTheta - playerDirection.y * sinTheta;
            playerDirection.y = oldDirX * sinTheta + playerDirection.y * cosTheta;

            float oldPlaneX = cameraPlane.x;
            cameraPlane.x = cameraPlane.x*cosTheta - cameraPlane.y * sinTheta;
            cameraPlane.y = oldPlaneX*sinTheta + cameraPlane.y *cosTheta;

        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
            float cosTheta = cos(-rotationSpeed  * deltaTime);
            float sinTheta = sin(-rotationSpeed  * deltaTime);
            float oldDirX = playerDirection.x;

            playerDirection.x = playerDirection.x * cosTheta - playerDirection.y * sinTheta;
            playerDirection.y = oldDirX * sinTheta + playerDirection.y * cosTheta;

            float oldPlaneX = cameraPlane.x;
            cameraPlane.x = cameraPlane.x*cosTheta - cameraPlane.y * sinTheta;
            cameraPlane.y = oldPlaneX*sinTheta + cameraPlane.y *cosTheta;

        }
        
        
        if(Keyboard::isKeyPressed(Keyboard::Key::W)) {
            playerVelocity =playerDirection*playerSpeed*deltaTime;
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
            playerVelocity = -playerDirection * playerSpeed * deltaTime;
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
            Vector2f leftStrafe = Vector2f(-playerDirection.y, playerDirection.x);
            playerVelocity = leftStrafe * playerSpeed * deltaTime;

        }
        if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
            Vector2f rightStrafe = Vector2f(playerDirection.y, -playerDirection.x);
            playerVelocity = rightStrafe * playerSpeed * deltaTime;
        }
        
        Vector2f projectedPosition(playerPosition.x + playerVelocity.x, playerPosition.y + playerVelocity.y);
        if (map[(int)projectedPosition.y * mapWidth + (int)projectedPosition.x] != 1) {
            playerPosition += playerVelocity;
        }   
       
        window.clear();

        for (int x = 0; x < screenWidth; x++) {

            //$$ Ray_{Angle} = (Player_{angle} - \frac{FOV}{2})+\frac{x}{screenWidth} \times FOV$$

            //$$(Player_{angle} - \frac{FOV}{2})\rightarrow$$This is the value of the angle of the leftmost ray

            //$$\frac{x}{screenWidth} \times FOV \rightarrow$$ This is what we add to the leftmost ray angle value to

            // offset and add the actual angle as according to what column of the console we are on.

            float cameraX = 2*float(x)/float(screenWidth) -1; //range from -1 to 1? idk
            Vector2f rayDir = playerDirection + cameraPlane * cameraX;
            

            // $$\hat{V} = \begin{bmatrix}\sin{\theta}\\\cos{\theta}\\\end{bmatrix}$$

            Vector2f rayDirection = rayDir;
            Vector2f rayStart = playerPosition;
            Vector2f unitStepSize(sqrt(1+(rayDirection.y/rayDirection.x)*(rayDirection.y/rayDirection.x)),sqrt(1+(rayDirection.x/rayDirection.y)*(rayDirection.x/rayDirection.y)));
            
            Vector2i currentTile(int(rayStart.x),int(rayStart.y));
            Vector2f rayLength; //the x value is the ray lenght when we add the step.x, and the y value is when we add step.y
            Vector2f step;//negative or positive depending on direction

            if (rayDirection.x < 0) {
                step.x = -1;
                rayLength.x = (rayStart.x - currentTile.x) * unitStepSize.x;
            } else {
                step.x = 1; 
                rayLength.x = ((currentTile.x+1)-rayStart.x) * unitStepSize.x;
            }

            if (rayDirection.y < 0) {
                step.y = -1;
                rayLength.y = (rayStart.y - currentTile.y) * unitStepSize.y;
            } else {
                step.y = 1; 
                rayLength.y = ((currentTile.y+1)-rayStart.y) * unitStepSize.y;
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
                    if(map[int(currentTile.y)*mapWidth+int(currentTile.x)] == 1)
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
        if (fpsTimer >= 1) {
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
