// ColorPanel.cpp
#include "ColorPanel.hpp"
#include "UIHelper.hpp"
#include <algorithm>
#include <sstream>

ColorPanel::ColorPanel(const sf::Font& font) : font(font) {}

void ColorPanel::setColor(int index, const sf::Color& color) {
    if (currentColors && index >= 0 && index < 3) {
        (*currentColors)[index] = color;
    }
}


void ColorPanel::setTarget(std::array<sf::Color, 3>& colorSet) {
    currentColors = &colorSet;
}


void ColorPanel::draw(sf::RenderWindow& window) {
    if (!currentColors) return;
    drawColorButtons(window);
    drawSliders(window);
}

bool ColorPanel::handleEvent(const sf::Vector2i& mousePos, bool mousePressed, int& selectedColorIndex) {
    bool changed = false;
    for (int i = 0; i < 3; ++i) {
        sf::FloatRect btn(panelPos.x + i * 40, panelPos.y, 30, 30);
        if (btn.contains(static_cast<sf::Vector2f>(mousePos)) && mousePressed) {
            if (currentColorIndex != i) {
                currentColorIndex = i;
                selectedColorIndex = i;
                changed = true;
            }
        }
    }
    return changed;
}

bool ColorPanel::updateSliders(const sf::Vector2i& mousePos, bool mousePressed) {
    if (!currentColors) return false;

    bool colorChanged = false;
    sf::Color& col = (*currentColors)[currentColorIndex];
    int oldChannels[3] = { col.r, col.g, col.b };
    int newChannels[3] = { oldChannels[0], oldChannels[1], oldChannels[2] };

    for (int i = 0; i < 3; ++i) {
        sf::FloatRect bar(panelPos.x, panelPos.y + 50 + i * 30, sliderSize.x, sliderSize.y);
        if (mousePressed && bar.contains(static_cast<sf::Vector2f>(mousePos))) {
            float ratio = (mousePos.x - bar.left) / sliderSize.x;
            newChannels[i] = std::clamp(static_cast<int>(ratio * 255), 0, 255);
        }
    }

    // 変更があったかチェック
    for (int i = 0; i < 3; ++i) {
        if (oldChannels[i] != newChannels[i]) {
            colorChanged = true;
            break;
        }
    }

    if (colorChanged) {
        col.r = static_cast<sf::Uint8>(newChannels[0]);
        col.g = static_cast<sf::Uint8>(newChannels[1]);
        col.b = static_cast<sf::Uint8>(newChannels[2]);
    }

    return colorChanged;
}

std::array<sf::Color, 3> ColorPanel::getColorSet() const {
    if (!currentColors) {
        return { sf::Color::Red, sf::Color::Green, sf::Color::Blue };
    }
    return *currentColors;
}

void ColorPanel::drawColorButtons(sf::RenderWindow& window) {
    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape btn(sf::Vector2f(30, 30));
        btn.setPosition(panelPos.x + i * 40, panelPos.y);
        btn.setFillColor((*currentColors)[i]);
        btn.setOutlineColor(currentColorIndex == i ? sf::Color::White : sf::Color(100, 100, 100));
        btn.setOutlineThickness(2.f);
        window.draw(btn);
    }
}

void ColorPanel::drawSliders(sf::RenderWindow& window) {
    sf::Color& col = (*currentColors)[currentColorIndex];
    int values[3] = { col.r, col.g, col.b };
    std::string labels[3] = { "R", "G", "B" };

    for (int i = 0; i < 3; ++i) {
        float y = panelPos.y + 50 + i * 30;

        // スライダー背景
        sf::RectangleShape bar(sliderSize);
        bar.setPosition(panelPos.x, y);
        bar.setFillColor(sf::Color(60, 60, 60));
        window.draw(bar);

        // ノブ
        sf::RectangleShape knob({ 10, sliderSize.y });
        knob.setPosition(panelPos.x + (values[i] / 255.f) * (sliderSize.x - 10), y);
        knob.setFillColor(sf::Color::White);
        window.draw(knob);

        // ラベル
        std::stringstream ss;
        ss << labels[i] << ": " << values[i];
        drawText(window, font, ss.str(), 14, sf::Vector2f(panelPos.x, y - 18), sf::Color::White);
    }


}