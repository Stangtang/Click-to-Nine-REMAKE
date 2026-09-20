#include "raylib.h"
#include <iostream>
#include <string>

using namespace std;

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(800, 450, "Click to Nine - The Prequel");

    const int monitor = GetCurrentMonitor();
    const int displayWidth = GetMonitorWidth(monitor);
    const int displayHeight = GetMonitorHeight(monitor);
    const Vector2 monitorPos = GetMonitorPosition(monitor);

    SetWindowSize(displayWidth, displayHeight);
    SetWindowPosition(monitorPos.x, monitorPos.y);
    SetTargetFPS(GetMonitorRefreshRate(monitor));

    const int fontSize = 40;
    const float textSpacing = 2;
    Font font = LoadFontEx("calibri-regular.ttf", fontSize, NULL, 0);

    char clicks = '0';
    string clicksStr(1, clicks);
	const char* clicksCStr = clicksStr.c_str();
    Vector2 textSize = MeasureTextEx(font, clicksCStr, fontSize, textSpacing);
    Vector2 textPosition = {
        (displayWidth - textSize.x) / 2,
        (displayHeight - textSize.y) / 2
    };

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (clicks >= '9')
        {
            break;
		}

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            clicks++;
            clicksStr = (1, clicks);
            clicksCStr = clicksStr.c_str();
            textSize = MeasureTextEx(font, clicksCStr, fontSize, textSpacing);
            textPosition = {
                (displayWidth - textSize.x) / 2,
                (displayHeight - textSize.y) / 2
            };
        }

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        // DrawFPS(10, 10);

        DrawTextEx(font, clicksCStr, textPosition, fontSize, textSpacing, BLACK);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
