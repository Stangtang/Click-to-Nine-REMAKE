#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <algorithm>

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
    constexpr int fontSize = 40;
    constexpr float textSpacing = 2.0f;
    Font font = LoadFontEx("calibri-regular.ttf", fontSize, nullptr, 0);

    char clicks = '0';
	char clicksStr[2] = { clicks, '\0' };
    Vector2 textSize = MeasureTextEx(font, clicksStr, fontSize, textSpacing);
    Vector2 textPosition = {
        (displayWidth - textSize.x) / 2,
        (displayHeight - textSize.y) / 2
    };

    const Color numberColor = DARKGRAY;
    constexpr float opaqueAlpha = 1.0f;
    constexpr float startingAlpha = opaqueAlpha * 0.5f;
    constexpr float fadeTimeSeconds = 0.5f;
    constexpr float fadeSpeed = startingAlpha / fadeTimeSeconds;
    float alpha = 0;

    constexpr int numberFontSize = 3000;
    constexpr float numberTextSpacing = 0.0f;
    char numberText = '0';
    char numberTextStr[2] = { numberText, '\0' };
    Vector2 numberTextSize = MeasureTextEx(font, numberTextStr, numberFontSize, numberTextSpacing);
    Vector2 numberTextPosition = {
        (displayWidth - numberTextSize.x) / 2,
        (displayHeight - numberTextSize.y) / 2
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

            numberTextStr[0] = clicks;
            numberTextSize = MeasureTextEx(font, numberTextStr, numberFontSize, numberTextSpacing);
            numberTextPosition = {
                (displayWidth - numberTextSize.x) / 2,
                (displayHeight - numberTextSize.y) / 2
            };
            alpha = startingAlpha;
        }

        if (alpha > 0.0f) {
            alpha = max(alpha - fadeSpeed * GetFrameTime(), 0.0f);
        }
        Color transparentNumberColor = ColorAlpha(numberColor, alpha);

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        // DrawFPS(10, 10);

        DrawTextEx(font, clicksStr, textPosition, fontSize, textSpacing, BLACK);
        if (alpha > 0.0f) {
            DrawTextEx(font, numberTextStr, numberTextPosition, numberFontSize, numberTextSpacing, transparentNumberColor);
        }

        EndDrawing();
    }

    UnloadFont(font);

    CloseWindow();

    return 0;
}
