// ColorPanel.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <string>

// 前方宣言
class GlobalColorPalette;

class ColorPanel {
public:
    ColorPanel(const sf::Font& font);
    void draw(sf::RenderWindow& window);
    bool handleEvent(const sf::Vector2i& mousePos, bool mousePressed, int& selectedColorIndex);
    bool updateSliders(const sf::Vector2i& mousePos, bool mousePressed);
    std::array<sf::Color, 3> getColorSet() const;
    void setTarget(std::array<sf::Color, 3>& colorSet); // ★ 外部の色セットを参照設定
    void setPosition(const sf::Vector2f& pos) {
        this->panelPos = pos;
    }
    bool contains(const sf::Vector2f& point) const {
        return point.x >= panelPos.x && point.x <= panelPos.x + 120 &&
            point.y >= panelPos.y && point.y <= panelPos.y + 150;
    }

    // 現在選択中のカラーインデックスを取得
    int getCurrentColorIndex() const {
        return currentColorIndex;
    }

    // 指定インデックスの色を設定
    void setColor(int index, const sf::Color& color);

    // 現在選択中の色スロットに新しい色を設定
    void setCurrentColor(const sf::Color& color) {
        setColor(currentColorIndex, color);
    }

    // 新しいメソッド：グローバルカラーパレットとの連携
    void setGlobalColorPalette(GlobalColorPalette* globalPalette);

    // グローバルカラーインデックスを設定
    void setGlobalColorIndices(const std::array<int, 3>& indices);

    // 現在のグローバルカラーインデックスを取得
    std::array<int, 3> getGlobalColorIndices() const {
        return globalColorIndices;
    }

    // グローバルカラーから実際の色を更新
    void updateColorsFromGlobal();

    // 現在編集中の色をグローバルカラーパレットに反映
    void updateGlobalColorFromCurrent();

    // 現在選択中の色スロットのグローバルインデックスを変更
    void setCurrentSlotGlobalIndex(int globalIndex);

    // 現在選択中の色スロットのグローバルインデックスを取得
    int getCurrentSlotGlobalIndex() const {
        return globalColorIndices[currentColorIndex];
    }

private:
    std::array<sf::Color, 3>* currentColors = nullptr; // ★ 外部参照

    sf::Font font;
    sf::Vector2f panelPos = { 350, 60 };
    sf::Vector2f sliderSize = { 120, 16 };
    int currentColorIndex = 0;

    // 新しく追加：グローバルカラーパレットとの連携用
    std::array<int, 3> globalColorIndices = { 0, 1, 2 }; // デフォルト：最初の3色
    GlobalColorPalette* globalColorPalette = nullptr; // グローバルカラーパレットへの参照

    void drawSliders(sf::RenderWindow& window);
    void drawColorButtons(sf::RenderWindow& window);
};