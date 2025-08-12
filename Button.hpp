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
    bool isActiveState = false;  // �A�N�e�B�u��ԃt���O

    // �F��`
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
     * �{�^���̏�Ԃ��X�V
     * @param mousePos �}�E�X�ʒu
     */
    void update(const sf::Vector2i& mousePos) {
        sf::FloatRect bounds = shape.getGlobalBounds();
        isHovered = bounds.contains(static_cast<sf::Vector2f>(mousePos));

        // �z�o�[���̐F�ύX
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
        // �{�^���̏�Ԃɉ����ĐF������
        sf::Color buttonColor;
        if (isActiveState) {
            buttonColor = ACTIVE_COLOR;     // �A�N�e�B�u���F���邢��
        }
        else if (isHovered) {
            buttonColor = HOVER_COLOR;      // �z�o�[���F�����O���[
        }
        else {
            buttonColor = NORMAL_COLOR;     // �ʏ펞�F�_�[�N�O���[
        }

        shape.setFillColor(buttonColor);
        window.draw(shape);

        // �e�L�X�g�`��
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
     * �{�^���̈ʒu���擾
     * @return �{�^���̍���p�̍��W
     */
    sf::Vector2f getPosition() const {
        return position;
    }

    /**
     * �{�^���̒����ʒu���擾
     * @return �{�^���̒������W
     */
    sf::Vector2f getCenterPosition() const {
        return sf::Vector2f(
            position.x + size.x / 2.0f,
            position.y + size.y / 2.0f
        );
    }

    /**
     * �{�^���̃T�C�Y���擾
     * @return �{�^���̃T�C�Y
     */
    sf::Vector2f getSize() const {
        return size;
    }

    /**
     * �{�^���̋��E��`���擾
     * @return �{�^���̋��E��`
     */
    sf::FloatRect getBounds() const {
        return sf::FloatRect(position, size);
    }

    /**
     * �{�^���̈ʒu��ݒ�
     * @param newPos �V�����ʒu
     */
    void setPosition(const sf::Vector2f& newPos) {
        position = newPos;
        shape.setPosition(position);
        updateTextPosition();
    }

    /**
     * �e�L�X�g�̈ʒu���{�^�������ɍX�V
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
// �F��`�iButton.cpp�ɒǉ��j
const sf::Color Button::NORMAL_COLOR = sf::Color(70, 70, 70);
const sf::Color Button::HOVER_COLOR = sf::Color(90, 90, 90);
const sf::Color Button::ACTIVE_COLOR = sf::Color(100, 150, 255);
*/