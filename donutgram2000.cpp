#include <iostream>  // for standard input/output stream
#include <algorithm> // for std::clamp and std::fill
#include <cmath>      // for math functions like sin, cos, sqrt
#include <vector>     // for dynamic buffer storage
#include <windows.h>  // for console handling in Windows
#include <conio.h>    // for _kbhit and _getch
#include <thread>     // for std::this_thread::sleep_for
#include <chrono>     // for specifying durations

// Constants to define geometry and animation behavior
constexpr float kPi = 3.14159265f;                     // Constant for PI
constexpr float kThetaStep = 0.045f;                   // Step size for angle theta
constexpr float kPhiStep = 0.01f;                      // Step size for angle phi
constexpr float kDonutRadius = 2.1f;                   // Radius of the torus tube
constexpr float kCameraDistance = 5.0f;                // Distance of camera from origin
constexpr int kFrameDelayMs = 30;                      // Delay between frames in milliseconds
constexpr char luminanceRamp[] = " .,-~:;=!*#$@";       // Characters used for brightness levels
constexpr int kLuminanceLevels = sizeof(luminanceRamp) - 1;  // Number of luminance steps

// RAII class to automatically hide and restore cursor in the console
class ConsoleCursorGuard {
public:
    explicit ConsoleCursorGuard(HANDLE consoleHandle) : handle(consoleHandle) {
        GetConsoleCursorInfo(handle, &cursorInfo);
        cursorInfo.bVisible = FALSE;  // Hide cursor
        SetConsoleCursorInfo(handle, &cursorInfo);
    }
    ~ConsoleCursorGuard() {
        cursorInfo.bVisible = TRUE;   // Restore cursor
        SetConsoleCursorInfo(handle, &cursorInfo);
    }
private:
    HANDLE handle;
    CONSOLE_CURSOR_INFO cursorInfo;
};

// Display an ASCII intro splash screen until any key is pressed, then clear the screen manually
void showIntro(HANDLE console) {
    std::string intro = R"(
==========================================
        WELCOME TO DONUTGRAM 2000
    ASCII Torus Renderer & Spinner XT
==========================================

        Press any key to begin...
)";
    std::cout << intro;
    while (!_kbhit()) std::this_thread::sleep_for(std::chrono::milliseconds(100));
    _getch();

    // Clear screen manually (Windows style)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(console, &csbi);
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;
    COORD home = { 0, 0 };
    DWORD written;
    FillConsoleOutputCharacter(console, ' ', cells, home, &written);
    FillConsoleOutputAttribute(console, csbi.wAttributes, cells, home, &written);
    SetConsoleCursorPosition(console, home);
}

// Retrieve the console window's dimensions (width and height)
void getConsoleSize(HANDLE console, int& width, int& height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(console, &csbi)) {
        width = 80;
        height = 25;
    } else {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
}

// Render one frame of the spinning donut
void renderFrame(std::vector<char>& output, std::vector<float>& zbuffer, int width, int height,
                float scaleX, float scaleY, float A, float B) {
    std::fill(output.begin(), output.end(), ' ');         // Clear frame buffer
    std::fill(zbuffer.begin(), zbuffer.end(), 0.0f);       // Reset depth buffer
    const int bufferSize = width * height;

    // Light source direction vector (normalized)
    float lightX = 0.0f, lightY = 1.0f, lightZ = -1.0f;
    float lightLen = std::sqrt(lightX*lightX + lightY*lightY + lightZ*lightZ);
    lightX /= lightLen; lightY /= lightLen; lightZ /= lightLen;

    // Double angle loop: theta for the torus and phi for the circle being rotated
    for (float theta = 0.0f; theta < 2 * kPi; theta += kThetaStep) {
        for (float phi = 0.0f; phi < 2 * kPi; phi += kPhiStep) {
            // Precompute trigonometric values
            float sinA = sinf(A), cosA = cosf(A);
            float sinB = sinf(B), cosB = cosf(B);
            float sintheta = sinf(theta), costheta = cosf(theta);
            float sinphi = sinf(phi), cosphi = cosf(phi);

            // Coordinates of the 3D point on the torus
            float circlex = cosphi + kDonutRadius;
            float circley = sinphi;

            // 3D rotation transformation for the point
            float x = circlex * (cosB * costheta + sinA * sinB * sintheta) - circley * cosA * sinB;
            float y = circlex * (sinB * costheta - sinA * cosB * sintheta) + circley * cosA * cosB;
            float z = kCameraDistance + cosA * circlex * sintheta + circley * sinA;
            float ooz = 1.0f / z;  // One over z for perspective projection

            // Project x and y to screen coordinates
            int xp = static_cast<int>(width / 2 + scaleX * ooz * x);
            int yp = static_cast<int>(height / 2 - scaleY * ooz * y);
            int idx = xp + width * yp;

            // Compute the surface normal for lighting (approximation)
            float nx = costheta * cosphi;
            float ny = costheta * sinphi;
            float nz = sintheta;

            // Dot product between normal and light vector
            float dot = nx * lightX + ny * lightY + nz * lightZ;

            // If current pixel is closer than previous, write new pixel
            if (idx >= 0 && idx < bufferSize && ooz > zbuffer[idx]) {
                zbuffer[idx] = ooz;  // Update depth buffer
                int luminanceIndex = std::clamp(static_cast<int>((dot + 1.0f) * 0.5f * kLuminanceLevels), 0, kLuminanceLevels);
                output[idx] = luminanceRamp[luminanceIndex];  // Set brightness character
            }
        }
    }
}

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);     // Get console handle
    showIntro(hConsole);  // Display intro screen and clear after
    ConsoleCursorGuard cursorGuard(hConsole);              // Hide cursor automatically

    int screenWidth, screenHeight;
    getConsoleSize(hConsole, screenWidth, screenHeight);   // Get window size
    const int bufferSize = screenWidth * screenHeight;
    const float scaleX = static_cast<float>(screenWidth) / 2.0f;   // Scale factor for x
    const float scaleY = static_cast<float>(screenHeight) / 2.2f;  // Scale factor for y

    std::vector<char> output(bufferSize);    // Screen character buffer
    std::vector<float> zbuffer(bufferSize);  // Depth buffer (z-buffer)
    DWORD written;
    COORD origin = {0, 0};                   // Top-left screen position

    float A = 0.0f, B = 0.0f;                // Rotation angles
    float spinSpeedA = 0.045f;               // Speed of A rotation
    float spinSpeedB = 0.02f;                // Speed of B rotation

    while (true) {
        renderFrame(output, zbuffer, screenWidth, screenHeight, scaleX, scaleY, A, B);
        SetConsoleCursorPosition(hConsole, origin);  // Move cursor to top
        WriteConsoleOutputCharacterA(hConsole, output.data(), bufferSize, origin, &written);  // Draw frame
        A += spinSpeedA;  // Update rotation
        B += spinSpeedB;
        Sleep(kFrameDelayMs);  // Delay for frame rate

        // Handle user input for speed control or quit
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'q': return 0;  // Quit
                case 'w': spinSpeedA -= 0.01f; break;  // Increase vertical spin
                case 's': spinSpeedA += 0.01f; break;  // Decrease vertical spin
                case 'a': spinSpeedB -= 0.01f; break;  // Increase horizontal spin
                case 'd': spinSpeedB += 0.01f; break;  // Decrease horizontal spin
            }
        }
    }
}