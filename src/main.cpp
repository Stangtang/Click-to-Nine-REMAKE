#include "raylib.h"
#include "resource_dir.h"	// Utility header for SearchAndSetResourceDir

#include "tinyfiledialogs/tinyfiledialogs.h"

#include <array>
#include <cstddef>

namespace {
constexpr unsigned int kInitialWindowWidth = 100;
constexpr unsigned int kInitialWindowHeight = 100;
constexpr unsigned int kCounterFontSize = 50;
constexpr float kCounterSpacing = 2.0f;
constexpr unsigned int kNumberRasterSize = 2048;
constexpr float kNumberDrawSize = 2500.0f;
constexpr float kStartingAlpha = 0.5f;
constexpr float kFadeSpeed = kStartingAlpha / 0.5f;
constexpr unsigned int kWinningClickCount = 9;

struct DigitLayout {
    Vector2 counterPosition;
    Vector2 numberPosition;
};

Vector2 CenterGlyph(const Font& font, const int& codepoint, const float& drawSize, const int& screenWidth, const int& screenHeight) {
    const int index = GetGlyphIndex(font, codepoint);
    const float scale = drawSize / static_cast<float>(font.baseSize);
    const GlyphInfo& glyph = font.glyphs[index];
    const Rectangle& bounds = font.recs[index];

    return {
        (static_cast<float>(screenWidth) - bounds.width * scale) * 0.5f - static_cast<float>(glyph.offsetX) * scale,
        (static_cast<float>(screenHeight) - bounds.height * scale) * 0.5f - static_cast<float>(glyph.offsetY) * scale
    };
}

std::array<DigitLayout, 10> BuildDigitLayouts(const Font& counterFont, const Font& numberFont, const int& screenWidth, const int& screenHeight) {
    std::array<DigitLayout, 10> layouts{};

    for (std::size_t digit = 0; digit < layouts.size(); digit++) {
        const int codepoint = '0' + static_cast<int>(digit);
        const char text[2] = {static_cast<char>(codepoint), '\0'};
        const Vector2 textSize = MeasureTextEx(counterFont, text, static_cast<float>(kCounterFontSize), kCounterSpacing);

        layouts[digit] = {
            {(static_cast<float>(screenWidth) - textSize.x) * 0.5f, (static_cast<float>(screenHeight) - textSize.y) * 0.5f},
            CenterGlyph(numberFont, codepoint, kNumberDrawSize, screenWidth, screenHeight)
        };
    }

    return layouts;
}

} // namespace

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(kInitialWindowWidth, kInitialWindowHeight, "Click to Nine - The Prequel");

    const int monitor = GetCurrentMonitor();
    const int displayWidth = GetMonitorWidth(monitor);
    const int displayHeight = GetMonitorHeight(monitor);
    const Vector2 monitorPosition = GetMonitorPosition(monitor);

    SetWindowSize(displayWidth, displayHeight);
    SetWindowPosition(static_cast<int>(monitorPosition.x), static_cast<int>(monitorPosition.y));

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    SearchAndSetResourceDir("resources");

    const std::array<int, 10> digitCodepoints = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    Font counterFont = LoadFontEx("calibri-regular.ttf", kCounterFontSize, digitCodepoints.data(), static_cast<int>(digitCodepoints.size()));
    Font numberFont = LoadFontEx("calibri-regular.ttf", kNumberRasterSize, digitCodepoints.data(), static_cast<int>(digitCodepoints.size()));
    SetTextureFilter(numberFont.texture, TEXTURE_FILTER_BILINEAR);

    const std::array<DigitLayout, 10> layouts = BuildDigitLayouts(counterFont, numberFont, screenWidth, screenHeight);

    const Color backgroundColor = LIGHTGRAY;
    const Color counterColor = BLACK;
    const Color numberColor = DARKGRAY;
    float currentAlpha = 0.0f;

    unsigned int clickCount = 0;

    EnableEventWaiting();

    while (!WindowShouldClose() && clickCount < kWinningClickCount) {
        if (currentAlpha > 0.0f) {
            currentAlpha -= kFadeSpeed * GetFrameTime();
            if (currentAlpha < 0.0f) {
                currentAlpha = 0.0f;
                EnableEventWaiting();
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            clickCount++;
            currentAlpha = kStartingAlpha;
            DisableEventWaiting();
        }

        const DigitLayout& layout = layouts[clickCount];
        const int codepoint = '0' + clickCount;

        BeginDrawing();
        ClearBackground(backgroundColor);

        DrawTextCodepoint(counterFont, codepoint, layout.counterPosition, static_cast<float>(kCounterFontSize), counterColor);

        if (currentAlpha > 0.0f) {
            DrawTextCodepoint(numberFont, codepoint, layout.numberPosition, kNumberDrawSize, ColorAlpha(numberColor, currentAlpha));
        }

        EndDrawing();
    }

    UnloadFont(counterFont);
    UnloadFont(numberFont);

    CloseWindow();
}
