// ColorPanel.cpp
#include "ColorPanel.hpp"
#include "UIHelper.hpp"
#include "GlobalColorPalette.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

ColorPanel::ColorPanel(const sf::Font& font) : font(font) {}

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
    for (int i = 0; i < 4; ++i) { // 3����4�ɕύX
        sf::FloatRect btn(panelPos.x + i * 35, panelPos.y, 30, 30); // �Ԋu��35�ɒ���
        if (btn.contains(static_cast<sf::Vector2f>(mousePos)) && mousePressed) {
            if (currentColorIndex != i) {
                currentColorIndex = i;
                selectedColorIndex = i;
                changed = true;

                // ���O�o�́i�f�o�b�O�p�j
                if (i == 3) {
                    std::cout << "Transparent color selected" << std::endl;
                }
                else {
                    std::cout << "Color slot " << i << " selected" << std::endl;
                }
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

    // �ύX�����������`�F�b�N
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
    for (int i = 0; i < 4; ++i) { // 3����4�ɕύX
        sf::RectangleShape btn(sf::Vector2f(30, 30));
        btn.setPosition(panelPos.x + i * 35, panelPos.y); // �Ԋu��40����35�ɒ���

        if (i == 3) {
            // 4�ڂ̃{�^���F�����F�i�`�F�b�J�[�{�[�h�p�^�[���j
            drawTransparentButton(window, panelPos.x + i * 35, panelPos.y);
        }
        else {
            // 1-3�ڂ̃{�^���F�ʏ�̐F
            btn.setFillColor((*currentColors)[i]);
            window.draw(btn);
        }

        // �I��g�̕`��
        btn.setFillColor(sf::Color::Transparent);
        btn.setOutlineColor(currentColorIndex == i ? sf::Color::White : sf::Color(100, 100, 100));
        btn.setOutlineThickness(2.f);
        window.draw(btn);
    }
}

void ColorPanel::drawTransparentButton(sf::RenderWindow& window, float x, float y) {
    const int checkSize = 6; // �`�F�b�J�[�{�[�h��1�}�X�̃T�C�Y
    sf::Color color1(240, 240, 240); // ���邢�O���[
    sf::Color color2(200, 200, 200); // �Â��O���[

    // 30x30�̃{�^�����Ƀ`�F�b�J�[�{�[�h�p�^�[����`��
    for (int cy = 0; cy < 30; cy += checkSize) {
        for (int cx = 0; cx < 30; cx += checkSize) {
            // �`�F�b�J�[�{�[�h�p�^�[���̐F������
            sf::Color checkColor = ((cx / checkSize + cy / checkSize) % 2 == 0) ? color1 : color2;

            // �����ȋ�`��`��
            sf::RectangleShape check;
            check.setSize(sf::Vector2f(
                std::min(checkSize, 30 - cx),  // �{�^���̋��E�𒴂��Ȃ��悤����
                std::min(checkSize, 30 - cy)
            ));
            check.setPosition(x + cx, y + cy);
            check.setFillColor(checkColor);
            window.draw(check);
        }
    }
}


void ColorPanel::drawSliders(sf::RenderWindow& window) {
    sf::Color& col = (*currentColors)[currentColorIndex];
    int values[3] = { col.r, col.g, col.b };
    std::string labels[3] = { "R", "G", "B" };

    for (int i = 0; i < 3; ++i) {
        float y = panelPos.y + 50 + i * 30;

        // �X���C�_�[�w�i
        sf::RectangleShape bar(sliderSize);
        bar.setPosition(panelPos.x, y);
        bar.setFillColor(sf::Color(60, 60, 60));
        window.draw(bar);

        // �m�u
        sf::RectangleShape knob({ 10, sliderSize.y });
        knob.setPosition(panelPos.x + (values[i] / 255.f) * (sliderSize.x - 10), y);
        knob.setFillColor(sf::Color::White);
        window.draw(knob);

        // ���x��
        std::stringstream ss;
        ss << labels[i] << ": " << values[i];
        drawText(window, font, ss.str(), 14, sf::Vector2f(panelPos.x, y - 18), sf::Color::White);
    }
}

// �V�������\�b�h�̎���

void ColorPanel::setColor(int index, const sf::Color& color) {
    if (currentColors && index >= 0 && index < 3) {
        (*currentColors)[index] = color;
    }
}

void ColorPanel::setGlobalColorPalette(GlobalColorPalette* globalPalette) {
    globalColorPalette = globalPalette;
    updateColorsFromGlobal(); // �ݒ莞�ɐF���X�V
}

void ColorPanel::setGlobalColorIndices(const std::array<int, 3>& indices) {
    globalColorIndices = indices;
    updateColorsFromGlobal(); // �O���[�o���J���[������ۂ̐F���X�V
}

void ColorPanel::updateColorsFromGlobal() {
    if (globalColorPalette && currentColors) {
        for (int i = 0; i < 3; ++i) {
            int globalIndex = globalColorIndices[i];
            if (globalIndex >= 0 && globalIndex < 16) {
                (*currentColors)[i] = globalColorPalette->getColor(globalIndex);
            }
        }
    }
}

void ColorPanel::updateGlobalColorFromCurrent() {
    if (globalColorPalette && currentColors) {
        int globalIndex = globalColorIndices[currentColorIndex];
        if (globalIndex >= 0 && globalIndex < 16) {
            globalColorPalette->setColor(globalIndex, (*currentColors)[currentColorIndex]);
        }
    }
}

void ColorPanel::setCurrentSlotGlobalIndex(int globalIndex) {
    if (globalIndex >= 0 && globalIndex < 16) {
        globalColorIndices[currentColorIndex] = globalIndex;
        updateColorsFromGlobal(); // �F���X�V
    }
}