
// UIHelper.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <string>

inline void drawText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    unsigned int size,
    sf::Vector2f position,
    sf::Color color = sf::Color::White
) {
    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(color);
    sfText.setPosition(position);
    window.draw(sfText);
}
