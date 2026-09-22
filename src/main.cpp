#include "raylib.h"
#include "resource_dir.h"
#include "tinyfiledialogs/tinyfiledialogs.h"

#include <array>
#include <cstddef>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

namespace {

constexpr unsigned int kCounterRasterSize = 64;
constexpr float kCounterDrawSize = 50.0f;
constexpr unsigned int kNumberRasterSize = 2048;
constexpr float kNumberDrawSize = 2500.0f;
constexpr float kNumberStartingAlpha = 0.5f;
constexpr float kNumberFadeSpeed = kNumberStartingAlpha / 0.5f;
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

std::array<DigitLayout, kWinningClickCount + 1> BuildDigitLayouts(const Font& counterFont, const Font& numberFont, const int& screenWidth, const int& screenHeight) {
    std::array<DigitLayout, kWinningClickCount + 1> layouts{};
    for (std::size_t digit = 0; digit < layouts.size(); digit++) {
        const int codepoint = '0' + static_cast<int>(digit);
        const char text[2] = {static_cast<char>(codepoint), '\0'};
        layouts[digit] = {
            CenterGlyph(counterFont, codepoint, kCounterDrawSize, screenWidth, screenHeight),
            CenterGlyph(numberFont, codepoint, kNumberDrawSize, screenWidth, screenHeight)
        };
    }
    return layouts;
}

unsigned int LoadClickCount(const std::filesystem::path& savePath) {
    std::ifstream saveFile(savePath);
    if (!saveFile) {
        return 0;
    }

    unsigned int clickCount = 0;
    if (!(saveFile >> clickCount) || clickCount >= kWinningClickCount) {
        std::cerr << "Ignoring invalid save file: " << savePath << '\n';
        return 0;
    }

    saveFile >> std::ws;
    if (!saveFile.eof()) {
        std::cerr << "Ignoring invalid save file: " << savePath << '\n';
        return 0;
    }

    return clickCount;
}

bool SaveClickCount(const std::filesystem::path& savePath, const unsigned int& clickCount) {
    std::error_code error;
    std::filesystem::create_directories(savePath.parent_path(), error);
    if (error) {
        std::cerr << "Could not create save directory: " << error.message() << '\n';
        return false;
    }

    std::filesystem::path temporaryPath = savePath;
    temporaryPath += ".tmp";

    {
        std::ofstream saveFile(temporaryPath, std::ios::trunc);
        if (!saveFile) {
            std::cerr << "Could not open " << temporaryPath << '\n';
            return false;
        }

        saveFile << clickCount << '\n';
        saveFile.close();
        if (!saveFile) {
            std::cerr << "Error writing save file\n";
            std::filesystem::remove(temporaryPath, error);
            return false;
        }
    }

    std::filesystem::rename(temporaryPath, savePath, error);
    if (!error) {
        return true;
    }

    error.clear();
    std::filesystem::remove(savePath, error);
    if (error) {
        std::cerr << "Could not replace " << savePath << ": " << error.message() << '\n';
        std::filesystem::remove(temporaryPath, error);
        return false;
    }

    std::filesystem::rename(temporaryPath, savePath, error);
    if (error) {
        std::cerr << "Could not replace " << savePath << ": " << error.message() << '\n';
        std::filesystem::remove(temporaryPath, error);
        return false;
    }

    return true;
}

} // namespace

int main() {
    const int monitor = GetCurrentMonitor();
    const int displayWidth = GetMonitorWidth(monitor);
    const int displayHeight = GetMonitorHeight(monitor);
    const Vector2 monitorPosition = GetMonitorPosition(monitor);

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
    InitWindow(displayWidth, displayHeight, "Click to Nine - The Prequel");
    SetWindowPosition(static_cast<int>(monitorPosition.x), static_cast<int>(monitorPosition.y));

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    SearchAndSetResourceDir("resources");

    const std::array<int, kWinningClickCount + 1> digitCodepoints = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    Font counterFont = LoadFontEx("calibri-regular.ttf", kCounterRasterSize, digitCodepoints.data(), static_cast<int>(digitCodepoints.size()));
    SetTextureFilter(counterFont.texture, TEXTURE_FILTER_BILINEAR);
    Font numberFont = LoadFontEx("calibri-regular.ttf", kNumberRasterSize, digitCodepoints.data(), static_cast<int>(digitCodepoints.size()));
    SetTextureFilter(numberFont.texture, TEXTURE_FILTER_BILINEAR);

    const std::array<DigitLayout, kWinningClickCount + 1> layouts = BuildDigitLayouts(counterFont, numberFont, screenWidth, screenHeight);

    const Color backgroundColor = LIGHTGRAY;
    const Color counterColor = BLACK;
    const Color numberColor = DARKGRAY;
    float currentAlpha = 0.0f;

    InitAudioDevice();
    std::array<Sound, kWinningClickCount> clickSoundAliases{};
    clickSoundAliases.front() = LoadSound("click-sound-cut.mp3");
    for (std::size_t i = 1; i < clickSoundAliases.size(); i++) {
        clickSoundAliases[i] = LoadSoundAlias(clickSoundAliases.front());
    }
    std::size_t currentClickSound = 0;

    const std::filesystem::path savePath = "../save/SAVE.dat";
    unsigned int clickCount = LoadClickCount(savePath);

    EnableEventWaiting();

    while (!WindowShouldClose() && clickCount < kWinningClickCount) {
        if (currentAlpha > 0.0f) {
            currentAlpha -= kNumberFadeSpeed * GetFrameTime();
            if (currentAlpha < 0.0f) {
                currentAlpha = 0.0f;
                EnableEventWaiting();
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            clickCount++;
            currentAlpha = kNumberStartingAlpha;
            PlaySound(clickSoundAliases[currentClickSound]);
            currentClickSound = (currentClickSound + 1) % clickSoundAliases.size();
            DisableEventWaiting();
        }

        const DigitLayout& layout = layouts[clickCount];
        const int codepoint = '0' + clickCount;

        BeginDrawing();
        ClearBackground(backgroundColor);

        DrawTextCodepoint(counterFont, codepoint, layout.counterPosition, kCounterDrawSize, counterColor);
        if (currentAlpha > 0.0f) {
            DrawTextCodepoint(numberFont, codepoint, layout.numberPosition, kNumberDrawSize, ColorAlpha(numberColor, currentAlpha));
        }

        EndDrawing();
    }

    UnloadFont(counterFont);
    UnloadFont(numberFont);

    CloseWindow();

    int exitCode = 0;

    if (clickCount >= kWinningClickCount) {
        std::error_code error;
        std::filesystem::remove(savePath, error);
        if (error) {
            std::cerr << "Could not remove completed save: " << error.message() << '\n';
            exitCode = 1;
        }
    } else if (clickCount > 0) {
        const int exitChoice = tinyfd_messageBox(
            "Exit Prompt - Click to Nine (The Prequel)", // Title
            "Would you like to save your progress?", // Message
            "yesno", // Dialog type ("ok", "okcancel", "yesno", "yesnocancel")
            "question", // Icon type ("info", "warning", "error", "question")
            1 // Default button
        );
        if (exitChoice && !SaveClickCount(savePath, clickCount)) {
            exitCode = 1;
        }
    }

    // finish playing sounds
    while (true) {
        bool anySoundPlaying = false;
        for (const Sound& sound : clickSoundAliases) {
            if (IsSoundPlaying(sound)) {
                anySoundPlaying = true;
                break;
            }
        }
        if (!anySoundPlaying) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (std::size_t i = 1; i < clickSoundAliases.size(); i++) {
        UnloadSoundAlias(clickSoundAliases[i]);
    }
    UnloadSound(clickSoundAliases.front());
    CloseAudioDevice();

    return exitCode;
}
