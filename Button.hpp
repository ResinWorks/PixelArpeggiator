class Button {
private:
    sf::RectangleShape shape;
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

    void update(const sf::Vector2i& mousePos) {
        sf::FloatRect bounds = shape.getGlobalBounds();
        isHovered = bounds.contains(static_cast<sf::Vector2f>(mousePos));
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
};

// 色定義（Button.cpp に追加）
const sf::Color Button::NORMAL_COLOR = sf::Color(70, 70, 70);
const sf::Color Button::HOVER_COLOR = sf::Color(90, 90, 90);
const sf::Color Button::ACTIVE_COLOR = sf::Color(100, 150, 255);