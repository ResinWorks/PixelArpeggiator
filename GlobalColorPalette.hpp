#pragma once
//===== GlobalColorPalette.hpp =====
#pragma once
#include <SFML/Graphics.hpp>
#include <array>

class GlobalColorPalette {
private:
    std::array<sf::Color, 16> colors;
    int selectedIndex;
    sf::Vector2f position;
    float colorBoxSize;

public:
    GlobalColorPalette(sf::Vector2f pos, float boxSize)
        : position(pos), colorBoxSize(boxSize), selectedIndex(0) {
        // �f�t�H���g16�F�p���b�g�̏�����
        colors = {
            sf::Color::Black,       // 0
            sf::Color::White,       // 1
            sf::Color::Red,         // 2
            sf::Color::Green,       // 3
            sf::Color::Blue,        // 4
            sf::Color::Yellow,      // 5
            sf::Color::Magenta,     // 6
            sf::Color::Cyan,        // 7
            sf::Color(128, 0, 0),   // 8 - Dark Red
            sf::Color(0, 128, 0),   // 9 - Dark Green
            sf::Color(0, 0, 128),   // 10 - Dark Blue
            sf::Color(128, 128, 0), // 11 - Olive
            sf::Color(128, 0, 128), // 12 - Purple
            sf::Color(0, 128, 128), // 13 - Teal
            sf::Color(128, 128, 128), // 14 - Gray
            sf::Color(192, 192, 192)  // 15 - Light Gray
        };
    }

    // �N���b�N����
    bool handleClick(const sf::Vector2i& mousePos) {
        for (int i = 0; i < 16; ++i) {
            float y = position.y + i * (colorBoxSize + 2);
            sf::FloatRect bounds(position.x, y, colorBoxSize, colorBoxSize);

            if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                selectedIndex = i;
                return true;
            }
        }
        return false;
    }

    // �`��
    void draw(sf::RenderWindow& window) {
        for (int i = 0; i < 16; ++i) {
            float y = position.y + i * (colorBoxSize + 2);

            // �J���[�{�b�N�X
            sf::RectangleShape colorBox(sf::Vector2f(colorBoxSize, colorBoxSize));
            colorBox.setPosition(position.x, y);
            colorBox.setFillColor(colors[i]);

            // �I�����̃n�C���C�g
            if (i == selectedIndex) {
                colorBox.setOutlineThickness(3);
                colorBox.setOutlineColor(sf::Color::Yellow);
            }
            else {
                colorBox.setOutlineThickness(1);
                colorBox.setOutlineColor(sf::Color(100, 100, 100));
            }

            window.draw(colorBox);
        }
    }

    // �F�擾�E�ݒ�
    sf::Color& getSelectedColor() {
        return colors[selectedIndex];
    }

    const sf::Color& getSelectedColor() const {
        return colors[selectedIndex];
    }

    sf::Color getColor(int index) const {
        if (index >= 0 && index < 16) {
            return colors[index];
        }
        return sf::Color::Black; // �f�t�H���g
    }

    void setColor(int index, const sf::Color& color) {
        if (index >= 0 && index < 16) {
            colors[index] = color;
        }
    }

    int getSelectedIndex() const {
        return selectedIndex;
    }

    void setSelectedIndex(int index) {
        if (index >= 0 && index < 16) {
            selectedIndex = index;
        }
    }

    const std::array<sf::Color, 16>& getAllColors() const {
        return colors;
    }

    // �ʒu�ݒ�
    void setPosition(const sf::Vector2f& pos) {
        position = pos;
    }
};