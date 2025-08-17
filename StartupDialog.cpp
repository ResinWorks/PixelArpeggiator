// ===== StartupDialog.cpp =====

#include "StartupDialog.hpp"
#include <iostream>
#include <algorithm>

StartupDialog::StartupDialog(sf::RenderWindow& window, sf::Font& font)
    : window(window), font(font) {

    // �_�C�A���O�{�b�N�X�̐ݒ�
    dialogBox.setSize(sf::Vector2f(600, 300));
    dialogBox.setPosition(600, 300); // ��ʒ����t��
    dialogBox.setFillColor(sf::Color(50, 50, 50));
    dialogBox.setOutlineThickness(2);
    dialogBox.setOutlineColor(sf::Color::White);

    // OK�{�^���̐ݒ�
    okButton.setSize(sf::Vector2f(80, 30));
    okButton.setPosition(850, 550);
    okButton.setFillColor(sf::Color(70, 150, 70));
    okButton.setOutlineThickness(1);
    okButton.setOutlineColor(sf::Color::White);

    // �L�����Z���{�^���̐ݒ�
    cancelButton.setSize(sf::Vector2f(80, 30));
    cancelButton.setPosition(950, 550);
    cancelButton.setFillColor(sf::Color(150, 70, 70));
    cancelButton.setOutlineThickness(1);
    cancelButton.setOutlineColor(sf::Color::White);

    // ���̓t�B�[���h�̐ݒ�
    tileSizeField.setSize(sf::Vector2f(100, 25));
    tileSizeField.setPosition(950, 380);
    tileSizeField.setFillColor(sf::Color(30, 30, 30));
    tileSizeField.setOutlineThickness(1);

    widthField.setSize(sf::Vector2f(100, 25));
    widthField.setPosition(950, 420);
    widthField.setFillColor(sf::Color(30, 30, 30));
    widthField.setOutlineThickness(1);

    heightField.setSize(sf::Vector2f(100, 25));
    heightField.setPosition(950, 460);
    heightField.setFillColor(sf::Color(30, 30, 30));
    heightField.setOutlineThickness(1);
}

bool StartupDialog::showDialog(CanvasSettings& outSettings) {
    bool dialogOpen = true;
    bool result = false;

    while (dialogOpen && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (handleEvent(event)) {
                dialogOpen = false;
                result = true;
                outSettings = settings;
            }

            // Escape�L�[�ŃL�����Z��
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                dialogOpen = false;
                result = false;
            }
        }

        // �`��
        window.clear(sf::Color(30, 30, 30));
        draw();
        window.display();
    }

    return result;
}

bool StartupDialog::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

        // OK�{�^���N���b�N
        if (okButton.getGlobalBounds().contains(mousePos)) {
            validateAndUpdateSettings();
            return true; // �_�C�A���O�I��
        }

        // �L�����Z���{�^���N���b�N
        if (cancelButton.getGlobalBounds().contains(mousePos)) {
            return false; // �L�����Z��
        }

        // ���̓t�B�[���h�N���b�N
        activeField = ActiveField::NONE;
        if (tileSizeField.getGlobalBounds().contains(mousePos)) {
            activeField = ActiveField::TILE_SIZE;
        }
        else if (widthField.getGlobalBounds().contains(mousePos)) {
            activeField = ActiveField::WIDTH;
        }
        else if (heightField.getGlobalBounds().contains(mousePos)) {
            activeField = ActiveField::HEIGHT;
        }
    }

    // �e�L�X�g���͏���
    if (event.type == sf::Event::TextEntered) {
        handleTextInput(event.text.unicode);
    }

    // Enter�L�[�Ŋm��
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
        validateAndUpdateSettings();
        return true;
    }

    return false;
}

void StartupDialog::handleTextInput(sf::Uint32 unicode) {
    if (activeField == ActiveField::NONE) return;

    std::string* currentInput = nullptr;
    switch (activeField) {
    case ActiveField::TILE_SIZE: currentInput = &tileSizeInput; break;
    case ActiveField::WIDTH: currentInput = &widthInput; break;
    case ActiveField::HEIGHT: currentInput = &heightInput; break;
    }

    if (!currentInput) return;

    // �o�b�N�X�y�[�X����
    if (unicode == 8 && !currentInput->empty()) {
        currentInput->pop_back();
    }
    // �����̂ݎ󂯓���
    else if (unicode >= '0' && unicode <= '9' && currentInput->length() < 4) {
        *currentInput += static_cast<char>(unicode);
    }
}

void StartupDialog::validateAndUpdateSettings() {
    // �^�C���T�C�Y�̌��؁i3-30�͈̔́j
    if (isValidInput(tileSizeInput, 3, 30)) {
        settings.tileSize = std::stoi(tileSizeInput);
    }

    // �L�����o�X���̌��؁i50-500�͈̔́j
    if (isValidInput(widthInput, 50, 500)) {
        settings.canvasWidth = std::stoi(widthInput);
    }

    // �L�����o�X�����̌��؁i50-500�͈̔́j
    if (isValidInput(heightInput, 50, 500)) {
        settings.canvasHeight = std::stoi(heightInput);
    }

    std::cout << "Settings: TileSize=" << settings.tileSize
        << ", Canvas=" << settings.canvasWidth << "x" << settings.canvasHeight << std::endl;
}

bool StartupDialog::isValidInput(const std::string& input, int min, int max) {
    if (input.empty()) return false;
    try {
        int value = std::stoi(input);
        return value >= min && value <= max;
    }
    catch (...) {
        return false;
    }
}

void StartupDialog::draw() {
    // �_�C�A���O�{�b�N�X
    window.draw(dialogBox);

    // �^�C�g��
    drawText("Canvas Settings", 800, 320, 20, sf::Color::White);

    // ���x��
    drawText("Tile Size (3-30):", 720, 385, 14, sf::Color::White);
    drawText("Width (50-500):", 720, 425, 14, sf::Color::White);
    drawText("Height (50-500):", 720, 465, 14, sf::Color::White);

    // ���̓t�B�[���h
    drawInputField(window, tileSizeField, tileSizeInput, activeField == ActiveField::TILE_SIZE);
    drawInputField(window, widthField, widthInput, activeField == ActiveField::WIDTH);
    drawInputField(window, heightField, heightInput, activeField == ActiveField::HEIGHT);

    // �{�^��
    window.draw(okButton);
    window.draw(cancelButton);
    drawText("OK", 875, 560, 14, sf::Color::White);
    drawText("Cancel", 970, 560, 14, sf::Color::White);

    // ���ӎ���
    drawText("Note: Tile Size should be divisible by 3 for best results", 720, 500, 12, sf::Color(200, 200, 100));
}

void StartupDialog::drawInputField(sf::RenderWindow& window, const sf::RectangleShape& field,
    const std::string& value, bool isActive) {
    // �t�B�[���h�̘g�F��ݒ�
    sf::RectangleShape fieldCopy = field;
    fieldCopy.setOutlineColor(isActive ? sf::Color::Yellow : sf::Color(100, 100, 100));
    window.draw(fieldCopy);

    // �e�L�X�g�\��
    sf::Vector2f pos = field.getPosition();
    drawText(value, pos.x + 5, pos.y + 5, 14, sf::Color::White);

    // �A�N�e�B�u���̓J�[�\���\��
    if (isActive) {
        sf::RectangleShape cursor(sf::Vector2f(1, 15));
        cursor.setPosition(pos.x + 8 + value.length() * 8, pos.y + 5);
        cursor.setFillColor(sf::Color::White);
        window.draw(cursor);
    }
}

void StartupDialog::drawText(const std::string& text, float x, float y, int size, sf::Color color) {
    sf::Text sfText(text, font, size);
    sfText.setPosition(x, y);
    sfText.setFillColor(color);
    window.draw(sfText);
}
