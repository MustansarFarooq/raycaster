#include "iostream"
#include "Windows.h"
#include "dataStructs.h"
#include "chrono"
#include "stdio.h"
#include "string"	
const float PI = 3.14159265358979323846;

using namespace std;
const int nScreenWidth = 174;
const int nScreenHeight = 41;

const int nMapWidth = 16;
const int nMapHeight = 16;


const float fDepth = 16.0f;
const float playerSpeed = 5.0f;
Vector2 vPlayer(8.0f, 8.0f);
float fPlayerAngle = 0.0f; // Player start angle
Vector2 playerVelocity(0.0f, 0.0f);

float fFOV = PI / 2.0f; // Field of view


int main()
{
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	
	wstring map;
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.........######";
	map += L"#..............#";
	map += L"#..###.........#";
	map += L"#..###.........#";
	map += L"#..###.........#";
	map += L"#..............#";
	map += L"#......#.......#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.........####.#";
	map += L"#..............#";
	map += L"################";


	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();
	// Display Frame
	while (1)
	{
		playerVelocity.x = 0;
		playerVelocity.y = 0;
		tp2 = chrono::system_clock::now();
		chrono::duration<float> deltaTime = tp2 - tp1;
		tp1 = tp2;
		float fDeltaTime = deltaTime.count();
		float fps = 1 / fDeltaTime;

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
		{
			fPlayerAngle -= 2.0f * fDeltaTime;
		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			fPlayerAngle += 2.0f * fDeltaTime;
		}
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			playerVelocity.x = sinf(fPlayerAngle) * playerSpeed * fDeltaTime;
			playerVelocity.y = cosf(fPlayerAngle) * playerSpeed * fDeltaTime;

		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			playerVelocity.x = sinf(fPlayerAngle) * -playerSpeed * fDeltaTime;
			playerVelocity.y = cosf(fPlayerAngle) * -playerSpeed * fDeltaTime;

		}
		Vector2 projectedPosition(vPlayer.x + playerVelocity.x, vPlayer.y + playerVelocity.y);
		if (map[(int)projectedPosition.y * nMapWidth + (int)projectedPosition.x] != '#')
		{
			vPlayer.add(playerVelocity);
		}



		for (int x = 0; x < nScreenWidth; x++)
		{
			
			//$$ Ray_{Angle} = (Player_{angle} - \frac{FOV}{2})+\frac{x}{screenWidth} \times FOV$$
			
		
			//$$(Player_{angle} - \frac{FOV}{2})\rightarrow$$This is the value of the angle of the leftmost ray
			
			
			//$$\frac{x}{screenWidth} \times FOV \rightarrow$$ This is what we add to the leftmost ray angle value to offset and add the actual angle as according to what column of the console we are on.
			
			float fRayAngle = (fPlayerAngle - fFOV / 2) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0.0f;
			bool bHitWall = false;

			//$$\hat{v} = \begin{bmatrix}\sin{\theta}\\\cos{\theta}\\\end{bmatrix}$$
			
			Vector2 rayV(sinf(fRayAngle), cosf(fRayAngle));

p

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += 0.1f;
				Vector2 testRay((int)(vPlayer.x + rayV.x * fDistanceToWall), (int)(vPlayer.y + rayV.y * fDistanceToWall));
				if (testRay.x < 0 || testRay.x >= nMapWidth || testRay.y < 0 || testRay.y >= nMapHeight)
				{
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else 
				{
					//so we go to the row first, then add the x which gets us the column
					if (map[testRay.y * nMapWidth + testRay.x] == '#')
					{
						bHitWall = true;
					}
				}

			}
			int nCeiling =(float)(nScreenHeight / 2.0f )- (nScreenHeight / (float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;


			short nshade = ' ';
			if (fDistanceToWall < fDepth / 4.0f)
			{
				nshade = 0x2588;
			}
			else if (fDistanceToWall < fDepth / 3.0f)
			{
				nshade = 0x2593;
			}
			else if (fDistanceToWall < fDepth / 2.0f)
			{
				nshade = 0x2592;
			}
			else if (fDistanceToWall < fDepth)
			{
				nshade = 0x2591;
			}


			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y < nCeiling) 
				{
					screen[y * nScreenWidth + x] = ' ';
				}
				else if (y > nCeiling && y <= nFloor)
				{
					screen[y * nScreenWidth + x] = nshade;
				}
				else
				{
					screen[y * nScreenWidth + x] = ' ';
				}
			}
		}

		screen[0] = 'F';
		screen[1] = 'P';
		screen[2] = 'S';
		screen[3] = ':';
		for (int i = 0; i < to_string(roundf(1/fDeltaTime*100)/100).length(); i++)
		{
			screen[i+4] = to_string(roundf(1 / fDeltaTime * 100) / 100)[i];
		}
		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		
	}
	return 0;
}


