//===== LargeTileSystem.hpp ��^�^�C���V�X�e�� =====
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include "DrawingTools.hpp"

/**
 * ��^�^�C�������Ǘ�����N���X
 * 2x2�܂���4x2�̃^�C���z�u���T�|�[�g
 */
class LargeTile {
public:
    // �^�C���z�u�^�C�v
    enum class ArrangementType {
        TILE_2x2,  // 2x2�z�u (4�^�C��)
        TILE_4x2   // 4x2�z�u (8�^�C��)
    };

    // �^�C���z�u���
    struct TileArrangement {
        ArrangementType type;
        std::vector<int> indices;  // �^�C���C���f�b�N�X�z��
    };

private:
    int largeTileId;              // ��^�^�C���ԍ��i0-11�j
    TileArrangement arrangement;  // �^�C���z�u���

    /**
    * **�C����**�F�ʒu���^�^�C���T�C�Y�ƃu���V�T�C�Y�ɃX�i�b�v
    * @param pos ���̈ʒu
    * @param view CanvasView�C���X�^���X
    * @param screenTileSize �X�N���[����̃^�C���T�C�Y
    * @param brushSize �u���V�T�C�Y
    * @param canvasTileSize �L�����o�X�̎��ۂ̃^�C���T�C�Y
    */
   


public:
    LargeTile(int id) : largeTileId(id) {
        calculateArrangement();
    }

    /**
     * ��^�^�C���ԍ�����^�C���z�u���v�Z
     */
    void calculateArrangement() {
        arrangement.indices.clear();

        if (largeTileId >= 0 && largeTileId <= 7) {
            // 2x2�p�^�[�� (4�^�C��)
            arrangement.type = ArrangementType::TILE_2x2;
            int baseRow = largeTileId / 2;
            int baseCol = largeTileId % 2;
            int baseIndex = baseRow * 8 + baseCol * 2;

            arrangement.indices = {
                baseIndex,      // topLeft
                baseIndex + 1,  // topRight
                baseIndex + 4,  // bottomLeft
                baseIndex + 5   // bottomRight
            };
        }
        else if (largeTileId >= 8 && largeTileId <= 11) {
            // 4x2�p�^�[�� (8�^�C��)
            arrangement.type = ArrangementType::TILE_4x2;
            int patternIndex = largeTileId - 8;
            int baseIndex = 32 + patternIndex * 8;

            arrangement.indices = {
                baseIndex,      // row0 col0
                baseIndex + 1,  // row0 col1
                baseIndex + 2,  // row0 col2
                baseIndex + 3,  // row0 col3
                baseIndex + 4,  // row1 col0
                baseIndex + 5,  // row1 col1
                baseIndex + 6,  // row1 col2
                baseIndex + 7   // row1 col3
            };
        }
    }

    // �Q�b�^�[
    int getId() const { return largeTileId; }
    const TileArrangement& getArrangement() const { return arrangement; }

    /**
     * �`��ʒu���v�Z
     */
    std::vector<sf::Vector2i> getDrawPositions(
        const sf::Vector2i& basePos,
        int tileSize
    ) const {
        std::vector<sf::Vector2i> positions;

        if (arrangement.type == ArrangementType::TILE_2x2) {
            // 2x2�z�u
            positions = {
                basePos,                                      // ����
                sf::Vector2i(basePos.x + tileSize, basePos.y),     // �E��
                sf::Vector2i(basePos.x, basePos.y + tileSize),     // ����
                sf::Vector2i(basePos.x + tileSize, basePos.y + tileSize) // �E��
            };
        }
        else {
            // 4x2�z�u
            for (int row = 0; row < 2; row++) {
                for (int col = 0; col < 4; col++) {
                    positions.push_back(sf::Vector2i(
                        basePos.x + col * tileSize,
                        basePos.y + row * tileSize
                    ));
                }
            }
        }
        return positions;
    }

    /**
     * �f�o�b�O���擾
     */
    std::string getDebugInfo() const {
        std::string info = "LargeTile[" + std::to_string(largeTileId) + "]: ";
        for (int index : arrangement.indices) {
            info += std::to_string(index) + ",";
        }
        info.pop_back(); // �Ō�̃J���}���폜
        return info;
    }
};

// ... (LargeTileTool��LargeTileManager�͕ύX�Ȃ�) ...
/**
 * ��^�^�C���c�[��
 * �N���b�N�ʒu��2x2�̑�^�^�C����z�u
 */
class LargeTileTool : public DrawingTool {
private:
    int currentLargeTileId = 0;  // ���ݑI�𒆂̑�^�^�C���ԍ�
    bool isDrawing = false;              // �`�撆�t���O
    sf::Vector2i startDrawPos;          // �`��J�n�ʒu
    sf::Vector2i lastPlacedPos;         // �Ō�ɔz�u�����ʒu

public:
    void onDrawStart(const sf::Vector2i& startPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize) override;

    void onDrawContinue(const sf::Vector2i& currentPos,
        const sf::Vector2i& lastPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize) override;

    void onDrawEnd(const sf::Vector2i& endPos,
        const sf::Vector2i& startPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize) override;

    void drawPreview(sf::RenderWindow& window,
        const sf::Vector2i& startPos,
        const sf::Vector2i& currentPos,
        const CanvasView& view,
        int brushSize) const override;

    void drawCursor(sf::RenderWindow& window,
        const sf::Vector2i& mousePos,
        const CanvasView& view,
        int brushSize,
        int tileSize) const override;

    std::string getToolName() const override { return "Large Tile"; }
    bool supportsContinuousDrawing() const override { return true; }

    /**
     * ��^�^�C���ԍ���ݒ�
     * @param largeTileId 0-11�̑�^�^�C���ԍ�
     */
    void setLargeTileId(int largeTileId) {
        if (largeTileId >= 0 && largeTileId <= 11) {
            currentLargeTileId = largeTileId;
        }
    }

    /**
     * ���݂̑�^�^�C���ԍ����擾
     */
    int getLargeTileId() const {
        return currentLargeTileId;
    }

private:
    /**
     * ��^�^�C�����L�����o�X�ɔz�u
     * @param basePos �z�u��ʒu�i�X�N���[�����W�j
     * @param canvas �`��ΏۃL�����o�X
     * @param view CanvasView�C���X�^���X
     */
     /**
       * �ʒu���^�^�C���T�C�Y�ƃu���V�T�C�Y�ɃX�i�b�v
       */
    sf::Vector2i snapPosition(const sf::Vector2i& pos,
        const CanvasView& view,
        int screenTileSize,
        int brushSize,
        int canvasTileSize) const;

    /**
       * �P�̑�^�^�C����z�u
       */
    void placeLargeTile(const sf::Vector2i& canvasPos,
        Canvas& canvas,
        const CanvasView& view) const;

    /**
     * �u���V�T�C�Y�ɉ����ĕ����̑�^�^�C����z�u
     */
    void placeLargeTiles(const sf::Vector2i& basePos,
        int brushSize,
        Canvas& canvas,
        const CanvasView& view) const;
};

/**
 * ��^�^�C���Ǘ��N���X
 * ��^�^�C���̑I���Ə��Ǘ�
 */
class LargeTileManager {
private:
    std::vector<LargeTile> largeTiles;
    int currentSelection = 0;

public:
    LargeTileManager() {
        // 12�̑�^�^�C����������
        for (int i = 0; i < 12; ++i) {
            largeTiles.emplace_back(i);
        }
    }

    /**
     * ��^�^�C����I��
     * @param id 0-11�̑�^�^�C���ԍ�
     */
    void selectLargeTile(int id) {
        if (id >= 0 && id < 12) {
            currentSelection = id;
        }
    }

    /**
     * ���ݑI�𒆂̑�^�^�C�����擾
     */
    const LargeTile& getCurrentLargeTile() const {
        return largeTiles[currentSelection];
    }

    /**
     * �w��ԍ��̑�^�^�C�����擾
     */
    const LargeTile& getLargeTile(int id) const {
        if (id >= 0 && id < 12) {
            return largeTiles[id];
        }
        return largeTiles[0];  // �t�H�[���o�b�N
    }

    /**
     * ���݂̑I��ԍ����擾
     */
    int getCurrentSelection() const {
        return currentSelection;
    }

    /**
     * �S��^�^�C���̏����f�o�b�O�o��
     */
    void printDebugInfo() const {
        for (const auto& tile : largeTiles) {
            std::cout << tile.getDebugInfo() << std::endl;
        }
    }
};