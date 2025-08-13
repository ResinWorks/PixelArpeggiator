#pragma once
// PatternGrid.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

class PatternGrid {
public:
    PatternGrid(int rows, int cols, int tileSize);

    void draw(sf::RenderWindow& window, const std::array<sf::Color, 3>& colorSet);
    bool handleClick(const sf::Vector2i& mousePos, int selectedColor);

    const std::vector<std::vector<int>>& getTiles() const {
        return tiles;
    }

    void setTiles(const std::vector<std::vector<int>>& newTiles) {
        if (newTiles.size() == tiles.size() && newTiles[0].size() == tiles[0].size()) {
            tiles = newTiles;
        }
    }

    void setPosition(const sf::Vector2f& pos) {
        this->position = pos;
    }

    // 新しいメソッド：グローバルカラーから3色を取得して描画
    void drawWithGlobalColors(sf::RenderWindow& window,
        const std::array<sf::Color, 16>& globalColors,
        const std::array<int, 3>& globalIndices) {
        std::array<sf::Color, 3> colorSet;
        for (int i = 0; i < 3; ++i) {
            int globalIndex = globalIndices[i];
            if (globalIndex >= 0 && globalIndex < 16) {
                colorSet[i] = globalColors[globalIndex];
            }
            else {
                colorSet[i] = sf::Color::Black; // フォールバック
            }
        }
        draw(window, colorSet);
    }

private:
    int rows, cols, tileSize;
    std::vector<std::vector<int>> tiles; // 色インデックス（0〜2）
    sf::Vector2f position;               // 左上位置
};