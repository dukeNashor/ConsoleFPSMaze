#include <iostream>
#include <Windows.h>

#include <chrono>
#include <vector>
#include <algorithm>

using namespace std;


int nScreenWidth = 120;
int nScreenHeight = 40;
int nMapHeight = 16;
int nMapWidth = 16;

float fPlayerX = 14.7f;
float fPlayerY = 5.09f;
float fPlayerA = 3.0f * 3.14159f / 2.0f;


float fFOV = 3.14159f / 4.0f;

float fDepth = 16.0f;

int main()
{
    std::cout << "Hello world!" << std::endl;
    
    auto* screen = new wchar_t[nScreenWidth * nScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    wstring map;

    map += L"################";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.....###......#";
    map += L"#.....###......#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#........#######";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();
    
    while (1)
    {
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTIme = elapsedTime.count();

        // Handle CCW rotation;
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        {
            fPlayerA -= (0.75f) * fElapsedTIme;
        }
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        {
            fPlayerA += (0.75f) * fElapsedTIme;
        }
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTIme;
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTIme;

            if (map[nMapWidth * (int)fPlayerY + (int) fPlayerX] == '#')
            {
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTIme;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTIme;
            }
        }
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTIme;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTIme;

            if (map[nMapWidth * (int)fPlayerY + (int) fPlayerX] == '#')
            {
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTIme;
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTIme;
            }
        }
        
        for (int x = 0; x < nScreenWidth; ++x)
        {
            // For each column, calculate the projected ray angle into world space;
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;
            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);
            
            while (!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += 0.1f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                // Test if ray is out of bounds;
                if (nTestX < 0 || nTestX > nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else
                {
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;

                        vector<pair<float, float>> p; // distance, dot

                        for (int tx = 0; tx < 2; ++tx)
                        {
                            for (int ty = 0; ty < 2; ++ty)
                            {
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vx*vx + vy*vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy /d);
                                p.emplace_back(make_pair(d, dot));
                            }
                        }

                        std::sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right){ return left.first < right.first; });

                        float fBound = 0.01f;
                        if (acos(p.at(0).second) < fBound) bBoundary = true;
                        if (acos(p.at(1).second) < fBound) bBoundary = true;
                        // if (acos(p.at(2).second) < fBound) bBoundary = true;

                    }
                }
                
            }

            // Calculate distance to ceiling and floor;
            int nCeiling = (float)(nScreenHeight / 2.0) - (float)nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            wchar_t nShade = ' ';

            if (fDistanceToWall <= fDepth / 4.0f)			nShade = 0x2588;
            else if (fDistanceToWall < fDepth / 3.0f)		nShade = 0x2593;
            else if (fDistanceToWall < fDepth / 2.0f)		nShade = 0x2592;
            else if (fDistanceToWall < fDepth)				nShade = 0x2591;
            else											nShade = ' ';


            if (bBoundary) nShade = ' ';

            // for each row
            for (int y = 0; y < nScreenHeight; ++y)
            {
                if (y <= nCeiling)
                    screen[y * nScreenWidth + x] = ' ';
                else if (y > nCeiling && y <= nFloor)
                {
                    screen[y * nScreenWidth + x] = nShade;
                }
                else
                {
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25f)				nShade = '#';
                    else if (b < 0.50f)			nShade = 'x';
                    else if (b < 0.75f)			nShade = '.';
                    else if (b < 0.90f)			nShade = '-';
                    else						nShade = ' ';
                    screen[y * nScreenWidth + x] = nShade;
                }
            }
            
        }

        swprintf_s(screen, 50, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2Ff", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTIme);

        // Display Map
        for (int nx = 0; nx < nMapWidth; nx++)
            for (int ny = 0; ny < nMapWidth; ny++)
            {
                screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + (nMapWidth - nx - 1)];
            }

        screen[((int)fPlayerY + 1) * nScreenWidth + (int)(nMapWidth -  fPlayerX)] = 'P';

        screen[nScreenHeight * nScreenWidth - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);

    }
    
    return 0;
}
