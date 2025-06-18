#pragma once
//===== LargeTilePaletteOverlay.hpp =====
#pragma once
#include <SFML/Graphics.hpp>
#include "LargeTileSystem.hpp"

/**
 * ��^�^�C���p���b�g�I�[�o�[���C
 * ������TilePalette�̏�ɑ�^�^�C���I��̈��\��
 */
class LargeTilePaletteOverlay {
private:
    sf::Vector2f palettePosition;    // TilePalette�̈ʒu
    float tileSize;                  // �P��^�C���̃T�C�Y
    int tilesPerRow;                 // 1�s������̃^�C�����i4�Œ�j
    bool isVisible = false;          // �I�[�o�[���C�̕\�����
    int selectedLargeTile = -1;      // �I�𒆂̑�^�^�C���ԍ�

    // ��^�^�C���̈�̒�`
    struct LargeTileRegion {
        sf::Vector2i topLeft;        // ����^�C�����W�i�s,��j
        sf::Vector2i size;           // �T�C�Y�i�s��,�񐔁j
        int largeTileId;             // ��^�^�C���ԍ�
    };

    std::vector<LargeTileRegion> largeTileRegions;

public:
    /**
     * �R���X�g���N�^
     * @param pos TilePalette�̈ʒu�iTilePalette::position�Ɠ����j
     * @param tSize �^�C���T�C�Y�iTilePalette::tileSize�Ɠ����j
     */
    LargeTilePaletteOverlay(const sf::Vector2f& pos, float tSize)
        : palettePosition(pos), tileSize(tSize), tilesPerRow(4) {
        initializeLargeTileRegions();
    }

    /**
     * ��^�^�C���̈��������
     * �z�u�\�Ɋ�Â��Ċe��^�^�C���̗̈���`
     */
    void initializeLargeTileRegions() {
        largeTileRegions.clear();

        // ��^�^�C�� 0-7�i2x2�p�^�[���j
        for (int i = 0; i < 8; ++i) {
            LargeTileRegion region;
            region.largeTileId = i;
            region.size = sf::Vector2i(2, 2);  // 2�sx2��

            // �z�u�v�Z�F0,1��0�s�ځA2,3��2�s��...
            int baseRow = (i / 2) * 2;  // 0,0,2,2,4,4,6,6
            int baseCol = (i % 2) * 2;  // 0,2,0,2,0,2,0,2

            region.topLeft = sf::Vector2i(baseRow, baseCol);
            largeTileRegions.push_back(region);
        }

        // ��^�^�C�� 8-11�i4x2�p�^�[���j
        for (int i = 8; i < 12; ++i) {
            LargeTileRegion region;
            region.largeTileId = i;
            region.size = sf::Vector2i(2, 4);  // 2�sx4��

            int baseRow = 8 + (i - 8) * 2;  // 8,10,12,14
            int baseCol = 0;

            region.topLeft = sf::Vector2i(baseRow, baseCol);
            largeTileRegions.push_back(region);
        }
    }

    /**
     * �I�[�o�[���C�̕\��/��\����؂�ւ�
     */
    void setVisible(bool visible) {
        isVisible = visible;
        if (!visible) {
            selectedLargeTile = -1;  // ��\�����͑I������
        }
    }

    /**
     * �I�[�o�[���C���\�������`�F�b�N
     */
    bool getVisible() const {
        return isVisible;
    }

    /**
     * �}�E�X�N���b�N����
     * @param mousePos �}�E�X���W
     * @return �N���b�N���ꂽ��^�^�C���ԍ��i-1: �N���b�N�Ȃ��j
     */
    int handleClick(const sf::Vector2i& mousePos) {
        if (!isVisible) return -1;

        // �}�E�X���W���^�C�����W�ɕϊ�
        sf::Vector2f localPos = static_cast<sf::Vector2f>(mousePos) - palettePosition;

        // �^�C�����W���v�Z�i5�s�N�Z���̊Ԋu���l���j
        int col = static_cast<int>(localPos.x / (tileSize + 5));
        int row = static_cast<int>(localPos.y / (tileSize + 5));

        // �e��^�^�C���̈���`�F�b�N
        for (const auto& region : largeTileRegions) {
            if (isInRegion(row, col, region)) {
                selectedLargeTile = region.largeTileId;
                return region.largeTileId;
            }
        }

        return -1;  // �ǂ̗̈�ɂ��Y�����Ȃ�
    }

    /**
     * �I�[�o�[���C�`��
     * @param window �`��E�B���h�E
     */
    void draw(sf::RenderWindow& window) const {
        if (!isVisible) return;

        // �������I�[�o�[���C�w�i
        sf::RectangleShape overlay(sf::Vector2f(
            tilesPerRow * (tileSize + 5) - 5,  // 4�񕪂̕�
            16 * (tileSize + 5) - 5            // 16�s���̍���
        ));
        overlay.setPosition(palettePosition);
        overlay.setFillColor(sf::Color(0, 0, 0, 120));  // ��������
        window.draw(overlay);

        // �e��^�^�C���̈��`��
        for (const auto& region : largeTileRegions) {
            drawLargeTileRegion(window, region);
        }

        // �I�𒆂̑�^�^�C�����n�C���C�g
        if (selectedLargeTile >= 0) {
            for (const auto& region : largeTileRegions) {
                if (region.largeTileId == selectedLargeTile) {
                    drawSelectionHighlight(window, region);
                    break;
                }
            }
        }
    }

    /**
     * ���ݑI�𒆂̑�^�^�C���ԍ����擾
     */
    int getSelectedLargeTile() const {
        return selectedLargeTile;
    }

    /**
     * ��^�^�C���ԍ��𒼐ڐݒ�
     */
    void setSelectedLargeTile(int largeTileId) {
        if (largeTileId >= 0 && largeTileId < 12) {
            selectedLargeTile = largeTileId;
        }
    }

private:
    /**
     * �w����W����^�^�C���̈�����`�F�b�N
     */
    bool isInRegion(int row, int col, const LargeTileRegion& region) const {
        return (row >= region.topLeft.x &&
            row < region.topLeft.x + region.size.x &&
            col >= region.topLeft.y &&
            col < region.topLeft.y + region.size.y);
    }

    /**
     * ��^�^�C���̈��`��
     */
    void drawLargeTileRegion(sf::RenderWindow& window, const LargeTileRegion& region) const {
        // �̈�̋��E����`��
        float regionWidth = region.size.y * (tileSize + 5) - 5;
        float regionHeight = region.size.x * (tileSize + 5) - 5;

        sf::Vector2f regionPos(
            palettePosition.x + region.topLeft.y * (tileSize + 5),
            palettePosition.y + region.topLeft.x * (tileSize + 5)
        );

        sf::RectangleShape regionRect(sf::Vector2f(regionWidth, regionHeight));
        regionRect.setPosition(regionPos);
        regionRect.setFillColor(sf::Color::Transparent);
        regionRect.setOutlineThickness(2);
        regionRect.setOutlineColor(sf::Color(255, 255, 0, 150));  // ���������F
        window.draw(regionRect);

        // ��^�^�C���ԍ���\��
        sf::CircleShape idMarker(8);
        idMarker.setOrigin(8, 8);
        idMarker.setPosition(
            regionPos.x + regionWidth / 2,
            regionPos.y + regionHeight / 2
        );
        idMarker.setFillColor(sf::Color(255, 0, 0, 180));
        idMarker.setOutlineThickness(1);
        idMarker.setOutlineColor(sf::Color::White);
        window.draw(idMarker);

        // �ԍ��e�L�X�g�i�ȗ��� - ���ۂɂ�sf::Text���g�p�j
        // �������ɂ̓t�H���g���K�v
    }

    /**
     * �I���n�C���C�g��`��
     */
    void drawSelectionHighlight(sf::RenderWindow& window, const LargeTileRegion& region) const {
        float regionWidth = region.size.y * (tileSize + 5) - 5;
        float regionHeight = region.size.x * (tileSize + 5) - 5;

        sf::Vector2f regionPos(
            palettePosition.x + region.topLeft.y * (tileSize + 5),
            palettePosition.y + region.topLeft.x * (tileSize + 5)
        );

        sf::RectangleShape highlight(sf::Vector2f(regionWidth, regionHeight));
        highlight.setPosition(regionPos);
        highlight.setFillColor(sf::Color(255, 255, 0, 80));  // ���������F�n�C���C�g
        highlight.setOutlineThickness(3);
        highlight.setOutlineColor(sf::Color::Yellow);
        window.draw(highlight);
    }
};