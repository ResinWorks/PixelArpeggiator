// PatternGrid.cpp
#include "PatternGrid.hpp"

PatternGrid::PatternGrid(int rows, int cols, int tileSize)
    : rows(rows), cols(cols), tileSize(tileSize), position(20.f, 20.f) {
    tiles.resize(rows, std::vector<int>(cols, 0)); // èâä˙íl0

}


void PatternGrid::draw(sf::RenderWindow& window, const std::array<sf::Color, 3>& colorSet) {
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            sf::RectangleShape rect(sf::Vector2f(tileSize - 2, tileSize - 2)); // 2pxåÑä‘
            rect.setPosition(position.x + x * tileSize, position.y + y * tileSize);
            rect.setFillColor(colorSet[tiles[y][x]]);
            window.draw(rect);
        }

    }
}

bool PatternGrid::handleClick(const sf::Vector2i& mousePos, int selectedColor) {
    sf::Vector2f localPos = sf::Vector2f(mousePos) - position;

    int x = static_cast<int>(localPos.x) / tileSize;
    int y = static_cast<int>(localPos.y) / tileSize;

    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        if (tiles[y][x] != selectedColor) {
            tiles[y][x] = selectedColor;
            return true;
        }
    }
    return false;
}
