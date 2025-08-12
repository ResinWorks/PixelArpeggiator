#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Button {
private:
    sf::RectangleShape shape;
    sf::Text buttonText;
    sf::Vector2f position;
    sf::Vector2f size;
    std::string text;
    bool isHovered = false;
    bool isActiveState = false;  // アクティブ状態フラグ

    // 色定義
    static const sf::Color NORMAL_COLOR;
    static const sf::Color HOVER_COLOR;
    static const sf::Color ACTIVE_COLOR;

public:
    Button(const std::string& buttonText, const sf::Vector2f& pos, const sf::Vector2f& buttonSize)
        : text(buttonText), position(pos), size(buttonSize) {
        shape.setPosition(pos);
        shape.setSize(buttonSize);
        shape.setFillColor(NORMAL_COLOR);
        shape.setOutlineThickness(1);
        shape.setOutlineColor(sf::Color(100, 100, 100));
    }

    void setActiveState(bool active) {
        isActiveState = active;
    }

    /**
     * ボタンの状態を更新
     * @param mousePos マウス位置
     */
    void update(const sf::Vector2i& mousePos) {
        sf::FloatRect bounds = shape.getGlobalBounds();
        isHovered = bounds.contains(static_cast<sf::Vector2f>(mousePos));

        // ホバー時の色変更
        if (isHovered) {
            shape.setFillColor(sf::Color(90, 90, 90));
        }
        else {
            shape.setFillColor(sf::Color(70, 70, 70));
        }
    }

    bool isClicked(const sf::Vector2i& mousePos, bool mousePressed) {
        if (mousePressed) {
            sf::FloatRect bounds = shape.getGlobalBounds();
            return bounds.contains(static_cast<sf::Vector2f>(mousePos));
        }
        return false;
    }

    void draw(sf::RenderWindow& window, const sf::Font& font) {
        // ボタンの状態に応じて色を決定
        sf::Color buttonColor;
        if (isActiveState) {
            buttonColor = ACTIVE_COLOR;     // アクティブ時：明るい青
        }
        else if (isHovered) {
            buttonColor = HOVER_COLOR;      // ホバー時：薄いグレー
        }
        else {
            buttonColor = NORMAL_COLOR;     // 通常時：ダークグレー
        }

        shape.setFillColor(buttonColor);
        window.draw(shape);

        // テキスト描画
        sf::Text buttonText(text, font, 12);
        sf::FloatRect textBounds = buttonText.getLocalBounds();
        buttonText.setPosition(
            position.x + (size.x - textBounds.width) / 2,
            position.y + (size.y - textBounds.height) / 2 - 2
        );
        buttonText.setFillColor(sf::Color::White);
        window.draw(buttonText);
    }

    /**
     * ボタンの位置を取得
     * @return ボタンの左上角の座標
     */
    sf::Vector2f getPosition() const {
        return position;
    }

    /**
     * ボタンの中央位置を取得
     * @return ボタンの中央座標
     */
    sf::Vector2f getCenterPosition() const {
        return sf::Vector2f(
            position.x + size.x / 2.0f,
            position.y + size.y / 2.0f
        );
    }

    /**
     * ボタンのサイズを取得
     * @return ボタンのサイズ
     */
    sf::Vector2f getSize() const {
        return size;
    }

    /**
     * ボタンの境界矩形を取得
     * @return ボタンの境界矩形
     */
    sf::FloatRect getBounds() const {
        return sf::FloatRect(position, size);
    }

    /**
     * ボタンの位置を設定
     * @param newPos 新しい位置
     */
    void setPosition(const sf::Vector2f& newPos) {
        position = newPos;
        shape.setPosition(position);
        updateTextPosition();
    }

    /**
     * テキストの位置をボタン中央に更新
     */
    void updateTextPosition() {
        sf::FloatRect textBounds = buttonText.getLocalBounds();
        sf::Vector2f textPos(
            position.x + (size.x - textBounds.width) / 2.0f - textBounds.left,
            position.y + (size.y - textBounds.height) / 2.0f - textBounds.top
        );
        buttonText.setPosition(textPos);
    }
};

/*
// 色定義（Button.cppに追加）
const sf::Color Button::NORMAL_COLOR = sf::Color(70, 70, 70);
const sf::Color Button::HOVER_COLOR = sf::Color(90, 90, 90);
const sf::Color Button::ACTIVE_COLOR = sf::Color(100, 150, 255);
*/