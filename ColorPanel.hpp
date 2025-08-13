
// ColorPanel.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <string>

class ColorPanel {
public:

    ColorPanel(const sf::Font& font);

    void draw(sf::RenderWindow& window);
    bool handleEvent(const sf::Vector2i& mousePos, bool mousePressed, int& selectedColorIndex);
    bool updateSliders(const sf::Vector2i& mousePos, bool mousePressed);
    std::array<sf::Color, 3> getColorSet() const;

    void setTarget(std::array<sf::Color, 3>& colorSet); // ★ 外部の色セットを参照設定

    void setPosition(const sf::Vector2f& pos) {
        this-> panelPos = pos;
    }

    bool contains(const sf::Vector2f& point) const {
		return point.x >= panelPos.x && point.x <= panelPos.x + 120 &&
			   point.y >= panelPos.y && point.y <= panelPos.y + 150;
	}

    // 現在の色インデックスを取得・設定
    int getCurrentColorIndex() const { return currentColorIndex; }
    void setColor(int index, const sf::Color& color);
    void setCurrentColor(const sf::Color& color) { setColor(currentColorIndex, color); }

private:
    /*
    struct RGBColor {
        int r, g, b;
        sf::Color toSFMLColor() const {
            return sf::Color(r, g, b);
        }
    };
    */

    //std::array<RGBColor, 3> colors;
    std::array<sf::Color, 3>* currentColors = nullptr; // ★ 外部参照
    
    sf::Font font;

    sf::Vector2f panelPos = { 350, 60 };
    sf::Vector2f sliderSize = { 120, 16 };

    int currentColorIndex = 0;

    void drawSliders(sf::RenderWindow& window);
    void drawColorButtons(sf::RenderWindow& window);



};
