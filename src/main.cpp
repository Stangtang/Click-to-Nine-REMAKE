#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

using namespace std;

Vector2 CenterGlyph(const Font& font, const int& codepoint, const float& drawSize) {
    const int index = GetGlyphIndex(font, codepoint);
    const float scale = drawSize / static_cast<float>(font.baseSize);

    const float glyphWidth = font.recs[index].width * scale;
    const float glyphHeight = font.recs[index].height * scale;
    const float offsetX = font.glyphs[index].offsetX * scale;
    const float offsetY = font.glyphs[index].offsetY * scale;

    return {
        (GetScreenWidth()  - glyphWidth)  * 0.5f - offsetX,
        (GetScreenHeight() - glyphHeight) * 0.5f - offsetY
    };
}

int main() {
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
    float currentAlpha = 0;

    int digitCodepoints[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    constexpr int numberRasterSize = 1024;
    Font numberFont = LoadFontEx("calibri-regular.ttf", numberRasterSize, digitCodepoints, sizeof(digitCodepoints) / sizeof(digitCodepoints[0]));
    SetTextureFilter(numberFont.texture, TEXTURE_FILTER_BILINEAR);
    constexpr int numberFontSize = 2500;
    constexpr float numberTextSpacing = 0.0f;
    char numberText = '0';
    char numberTextStr[2] = { numberText, '\0' };
    Vector2 numberTextSize = MeasureTextEx(numberFont, numberTextStr, numberFontSize, numberTextSpacing);
    Vector2 numberTextPosition = CenterGlyph(numberFont, numberTextStr[0], numberFontSize);

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
            numberTextSize = MeasureTextEx(numberFont, numberTextStr, numberFontSize, numberTextSpacing);
            numberTextPosition = CenterGlyph(numberFont, numberTextStr[0], numberFontSize);
            currentAlpha = startingAlpha;
        }

        if (currentAlpha > 0.0f) {
            currentAlpha = currentAlpha - fadeSpeed * GetFrameTime();
            if (currentAlpha < 0.0f) {
                currentAlpha = 0.0f;
            }
        }
        Color transparentNumberColor = ColorAlpha(numberColor, currentAlpha);

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        // DrawFPS(10, 10);

        DrawTextEx(font, clicksStr, textPosition, fontSize, textSpacing, BLACK);
        if (currentAlpha > 0.0f) {
            DrawTextEx(numberFont, numberTextStr, numberTextPosition, numberFontSize, numberTextSpacing, transparentNumberColor);
        }

        EndDrawing();
    }

    UnloadFont(font);
    UnloadFont(numberFont);

    CloseWindow();

    return 0;
}
