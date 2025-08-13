
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

    void setTarget(std::array<sf::Color, 3>& colorSet); // �� �O���̐F�Z�b�g���Q�Ɛݒ�

    void setPosition(const sf::Vector2f& pos) {
        this-> panelPos = pos;
    }

    bool contains(const sf::Vector2f& point) const {
		return point.x >= panelPos.x && point.x <= panelPos.x + 120 &&
			   point.y >= panelPos.y && point.y <= panelPos.y + 150;
	}

    // ���݂̐F�C���f�b�N�X���擾�E�ݒ�
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
    std::array<sf::Color, 3>* currentColors = nullptr; // �� �O���Q��
    
    sf::Font font;

    sf::Vector2f panelPos = { 350, 60 };
    sf::Vector2f sliderSize = { 120, 16 };

    int currentColorIndex = 0;

    void drawSliders(sf::RenderWindow& window);
    void drawColorButtons(sf::RenderWindow& window);



};
