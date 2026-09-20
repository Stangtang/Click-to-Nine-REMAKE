#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

using namespace std;

int main(void) {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(800, 450, "Click to Nine - The Prequel");

    const int monitor = GetCurrentMonitor();
    const int displayWidth = GetMonitorWidth(monitor);
    const int displayHeight = GetMonitorHeight(monitor);
    const Vector2 monitorPos = GetMonitorPosition(monitor);

    SetWindowSize(displayWidth, displayHeight);
    SetWindowPosition(monitorPos.x, monitorPos.y);
    SetTargetFPS(GetMonitorRefreshRate(monitor));

    SearchAndSetResourceDir("resources");
    const int fontSize = 40;
    const float textSpacing = 2;
    Font font = LoadFontEx("calibri-regular.ttf", fontSize, nullptr, 0);

    char clicks = '0';
	char clicksStr[2] = { clicks, '\0' };
    Vector2 textSize = MeasureTextEx(font, clicksStr, fontSize, textSpacing);
    Vector2 textPosition = {
        (displayWidth - textSize.x) / 2,
        (displayHeight - textSize.y) / 2
    };

    while (!WindowShouldClose()) { // Detect window close button or ESC key
        if (clicks >= '9') {
            break;
		}

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            clicks++;
            clicksStr[0] = clicks;
            textSize = MeasureTextEx(font, clicksStr, fontSize, textSpacing);
            textPosition = {
                (displayWidth - textSize.x) / 2,
                (displayHeight - textSize.y) / 2
            };
        }

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        // DrawFPS(10, 10);

        DrawTextEx(font, clicksStr, textPosition, fontSize, textSpacing, BLACK);

        EndDrawing();
    }

    UnloadFont(font);

    CloseWindow();

    return 0;
}
