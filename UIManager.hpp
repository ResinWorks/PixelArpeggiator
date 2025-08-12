#pragma once

#include <SFML/Graphics.hpp>
#include "Button.hpp"
#include "DrawingTools.hpp"
#include <vector>
#include <memory>
#include <algorithm>

#ifdef _WIN32
#define _USE_MATH_DEFINES
#include <cmath>
#else
#include <cmath>
#endif

// ボタンインデックス定義をグローバル名前空間に移動
enum class ButtonIndex {
    ADD_PATTERN = 0, SAVE_CHANGES = 1, SAVE_FILE = 2, LOAD_FILE = 3,
    EXPORT_PNG = 4, EXPORT_JPG = 5, TOGGLE_GRID = 6,
    BRUSH_SMALL = 7, BRUSH_MEDIUM = 8, BRUSH_LARGE = 9,
    TOOL_BRUSH = 10, TOOL_ERASER = 11, TOOL_LINE = 12,
    TOOL_CIRCLE = 13, TOOL_ELLIPSE = 14, TOOL_LARGE_TILE = 15,
    LARGE_TILE_PALETTE_TOGGLE = 16, ROTATE_BUTTON = 17, RESET_VIEW = 18
};

/**
 * UIManagerクラス - ボタンとスライダーの管理
 */
class UIManager {
private:
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<sf::RectangleShape> sliders;
    std::vector<sf::CircleShape> knobs;
    std::vector<sf::Text> labels;

public:
    UIManager(const sf::Font& font);

    // ボタン関連
    void initializeButtons();
    Button& getButton(ButtonIndex index);
    void updateButtons(const sf::Vector2i& mousePos);
    void drawButtons(sf::RenderWindow& window, const sf::Font& font, ToolManager::ToolType activeToolType,
        int currentBrushSize, bool largeTilePaletteVisible = false, int rotationDegrees = 0);

    // スライダー関連
    void initializeSliders(const sf::Font& font);
    void updateSliders(const sf::Vector2i& mousePos, bool mousePressed,
        float& gridSpacing, float& gridShrink, sf::Color& tileGridColor);
    void drawSliders(sf::RenderWindow& window, float gridSpacing, float gridShrink, const sf::Color& tileGridColor);

private:
    // ヘルパー関数
    bool isButtonActive(size_t i, ToolManager::ToolType activeToolType, int currentBrushSize,
        bool largeTilePaletteVisible, int rotationDegrees);
    void updateSlider(int index, const sf::Vector2i& mousePos, bool mousePressed, float& value);
    void updateColorSlider(int index, const sf::Vector2i& mousePos, bool mousePressed, sf::Uint8& colorComponent);
    void updateKnobPositions(float gridSpacing, float gridShrink, const sf::Color& tileGridColor);
    void drawColorPreview(sf::RenderWindow& window, const sf::Color& tileGridColor);
    void drawRotationIndicator(sf::RenderWindow& window, const sf::Font& font,
        const sf::Vector2f& buttonPos, int rotationDegrees);
    void drawRotationArrow(sf::RenderWindow& window, const sf::Vector2f& buttonPos, int rotationDegrees);
};