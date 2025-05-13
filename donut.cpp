#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <thread>

void showIntro() {
    std::string intro = R"(
==========================================
         WELCOME TO DONUTGRAM 2000
     ASCII Torus Renderer & Spinner XT
==========================================

         Press any key to begin...
)";

    std::cout << intro;
    while (!_kbhit()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    _getch();
    std::cout << "\x1b[2J\x1b[H"; // Clear screen
}

int main() {
    showIntro();

    float A = 0, B = 0;
    float spinSpeedA = 0.04f;
    float spinSpeedB = 0.02f;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    const int bufferSize = screenWidth * screenHeight;

    const float scaleX = screenWidth / 2.3f; // fill horizontally
    const float scaleY = screenHeight / 2.5f; // fill vertically

    std::vector<char> output(bufferSize);
    std::vector<float> zbuffer(bufferSize);

    DWORD written;
    COORD coord = {0, 0};

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    while (true) {
        std::fill(output.begin(), output.end(), ' ');
        std::fill(zbuffer.begin(), zbuffer.end(), 0.0f);

        for (float theta = 0; theta < 6.28f; theta += 0.05f) {
            for (float phi = 0; phi < 6.28f; phi += 0.015f) {
                float sinA = sin(A), cosA = cos(A);
                float sinB = sin(B), cosB = cos(B);
                float sintheta = sin(theta), costheta = cos(theta);
                float sinphi = sin(phi), cosphi = cos(phi);

                float circlex = cosphi + 2;
                float circley = sinphi;

                float x = circlex * (cosB * costheta + sinA * sinB * sintheta) - circley * cosA * sinB;
                float y = circlex * (sinB * costheta - sinA * cosB * sintheta) + circley * cosA * cosB;
                float z = 5 + cosA * circlex * sintheta + circley * sinA;
                float ooz = 1 / z;

                int xp = static_cast<int>(screenWidth / 2 + scaleX * ooz * x);
                int yp = static_cast<int>(screenHeight / 2 - scaleY * ooz * y);
                int idx = xp + screenWidth * yp;

                float L = cosphi * costheta * sinB - cosA * costheta * sinphi -
                          sinA * sintheta + cosB * (cosA * sintheta - costheta * sinphi * sinA);

                if (idx >= 0 && idx < bufferSize && ooz > zbuffer[idx]) {
                    zbuffer[idx] = ooz;
                    const char* lum = ".,-+~:;=!*#$@";
                    int luminance = std::clamp(static_cast<int>(L * 8), 0, 11);
                    output[idx] = lum[luminance];
                }
            }
        }

        SetConsoleCursorPosition(hConsole, coord);
        WriteConsoleOutputCharacterA(hConsole, output.data(), bufferSize, coord, &written);

        A += spinSpeedA;
        B += spinSpeedB;
        Sleep(30);

        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'q': goto end;
                case 'w': spinSpeedA -= 0.01f; break;
                case 's': spinSpeedA += 0.01f; break;
                case 'a': spinSpeedB -= 0.01f; break;
                case 'd': spinSpeedB += 0.01f; break;
            }
        }
    }

end:
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    return 0;
}