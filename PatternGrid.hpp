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

private:
    int rows, cols, tileSize;
    std::vector<std::vector<int>> tiles; // 色インデックス（0〜2）
    sf::Vector2f position;               // 左上位置
};
