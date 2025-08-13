// ColorPanel.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <string>

// �O���錾
class GlobalColorPalette;

class ColorPanel {
public:
    ColorPanel(const sf::Font& font);
    void draw(sf::RenderWindow& window);
    bool handleEvent(const sf::Vector2i& mousePos, bool mousePressed, int& selectedColorIndex);
    bool updateSliders(const sf::Vector2i& mousePos, bool mousePressed);
    std::array<sf::Color, 3> getColorSet() const;
    void setTarget(std::array<sf::Color, 3>& colorSet); // �� �O���̐F�Z�b�g���Q�Ɛݒ�
    void setPosition(const sf::Vector2f& pos) {
        this->panelPos = pos;
    }
    bool contains(const sf::Vector2f& point) const {
        return point.x >= panelPos.x && point.x <= panelPos.x + 120 &&
            point.y >= panelPos.y && point.y <= panelPos.y + 150;
    }

    // ���ݑI�𒆂̃J���[�C���f�b�N�X���擾
    int getCurrentColorIndex() const {
        return currentColorIndex;
    }

    // �w��C���f�b�N�X�̐F��ݒ�
    void setColor(int index, const sf::Color& color);

    // ���ݑI�𒆂̐F�X���b�g�ɐV�����F��ݒ�
    void setCurrentColor(const sf::Color& color) {
        setColor(currentColorIndex, color);
    }

    // �V�������\�b�h�F�O���[�o���J���[�p���b�g�Ƃ̘A�g
    void setGlobalColorPalette(GlobalColorPalette* globalPalette);

    // �O���[�o���J���[�C���f�b�N�X��ݒ�
    void setGlobalColorIndices(const std::array<int, 3>& indices);

    // ���݂̃O���[�o���J���[�C���f�b�N�X���擾
    std::array<int, 3> getGlobalColorIndices() const {
        return globalColorIndices;
    }

    // �O���[�o���J���[������ۂ̐F���X�V
    void updateColorsFromGlobal();

    // ���ݕҏW���̐F���O���[�o���J���[�p���b�g�ɔ��f
    void updateGlobalColorFromCurrent();

    // ���ݑI�𒆂̐F�X���b�g�̃O���[�o���C���f�b�N�X��ύX
    void setCurrentSlotGlobalIndex(int globalIndex);

    // ���ݑI�𒆂̐F�X���b�g�̃O���[�o���C���f�b�N�X���擾
    int getCurrentSlotGlobalIndex() const {
        return globalColorIndices[currentColorIndex];
    }

private:
    std::array<sf::Color, 3>* currentColors = nullptr; // �� �O���Q��

    sf::Font font;
    sf::Vector2f panelPos = { 350, 60 };
    sf::Vector2f sliderSize = { 120, 16 };
    int currentColorIndex = 0;

    // �V�����ǉ��F�O���[�o���J���[�p���b�g�Ƃ̘A�g�p
    std::array<int, 3> globalColorIndices = { 0, 1, 2 }; // �f�t�H���g�F�ŏ���3�F
    GlobalColorPalette* globalColorPalette = nullptr; // �O���[�o���J���[�p���b�g�ւ̎Q��

    void drawSliders(sf::RenderWindow& window);
    void drawColorButtons(sf::RenderWindow& window);
};