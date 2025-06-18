#pragma once
//===== DrawingManager.hpp�i�c�[���Ή��Łj =====
#pragma once
#include <SFML/Graphics.hpp>
#include "DrawingTools.hpp"
#include <memory>

// �O���錾
class Canvas;
class CanvasView;

/**
 * �`�揈���𓝍��Ǘ�����N���X
 * �V�����c�[���V�X�e���ɑΉ����A�l�X�ȕ`��c�[���𓝈�I�Ɉ���
 */
class DrawingManager {
private:
    // �`���ԊǗ�
    bool isDrawing = false;                    // ���ݕ`�撆���ǂ���
    sf::Vector2i lastMousePos;                // �O��̃}�E�X�ʒu�i�X�N���[�����W�j
    sf::Vector2i mouseDownPos;                // �}�E�X���������ʒu�i�X�N���[�����W�j
    bool hasMoved = false;                     // �}�E�X���ړ��������ǂ���
    static const int CLICK_THRESHOLD = 3;     // �N���b�N����̂������l�i�s�N�Z���j

    // �c�[���Ǘ�
    ToolManager toolManager;                   // �c�[���}�l�[�W���[

public:
    /**
     * �R���X�g���N�^
     */
    DrawingManager() = default;

    // ===== �`���ԊǗ� =====

    /**
     * �`��J�n
     * @param startPos �J�n�ʒu�i�X�N���[�����W�j
     * @param canvas �`��ΏۃL�����o�X
     * @param view CanvasView�C���X�^���X
     * @param patternIndex �I�����ꂽ�p�^�[���C���f�b�N�X
     * @param brushSize �u���V�T�C�Y
     */
    void startDrawing(const sf::Vector2i& startPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize);

    /**
     * �`��I��
     * @param endPos �I���ʒu�i�X�N���[�����W�j
     * @param canvas �`��ΏۃL�����o�X
     * @param view CanvasView�C���X�^���X
     * @param patternIndex �I�����ꂽ�p�^�[���C���f�b�N�X
     * @param brushSize �u���V�T�C�Y
     */
    void stopDrawing(const sf::Vector2i& endPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize);

    /**
     * �}�E�X�ړ��̍X�V
     * @param currentPos ���݂̃}�E�X�ʒu�i�X�N���[�����W�j
     * @param canvas �`��ΏۃL�����o�X
     * @param view CanvasView�C���X�^���X
     * @param patternIndex �I�����ꂽ�p�^�[���C���f�b�N�X
     * @param brushSize �u���V�T�C�Y
     */
    void updateMovement(const sf::Vector2i& currentPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize);

    /**
     * �Ō�̃}�E�X�ʒu���X�V
     * @param pos �V�����ʒu�i�X�N���[�����W�j
     */
    void updateLastMousePos(const sf::Vector2i& pos) {
        lastMousePos = pos;
    }

    // ===== ��Ԏ擾 =====

    /**
     * ���ݕ`�撆���ǂ���
     */
    bool getIsDrawing() const { return isDrawing; }

    /**
     * �}�E�X���ړ��������ǂ���
     */
    bool getHasMoved() const { return hasMoved; }

    /**
     * �}�E�X���������ʒu���擾
     */
    sf::Vector2i getMouseDownPos() const { return mouseDownPos; }

    /**
     * �O��̃}�E�X�ʒu���擾
     */
    sf::Vector2i getLastMousePos() const { return lastMousePos; }

    // ===== �c�[���Ǘ� =====

    /**
     * �c�[����؂�ւ�
     * @param toolType �؂�ւ���̃c�[���^�C�v
     */
    void setTool(ToolManager::ToolType toolType) {
        toolManager.setTool(toolType);
    }

    /**
     * ���݂̃c�[�����擾
     */
    DrawingTool* getCurrentTool() const {
        return toolManager.getCurrentTool();
    }

    /**
     * ���݂̃c�[���^�C�v���擾
     */
    ToolManager::ToolType getCurrentToolType() const {
        return toolManager.getCurrentToolType();
    }

    /**
     * ���݂̃c�[�������擾
     */
    std::string getCurrentToolName() const {
        return toolManager.getCurrentToolName();
    }

    // ===== �`��E�\���֘A =====

    /**
     * �J�[�\���`��
     * ���݂̃c�[���ɉ������J�[�\����\��
     * @param window �`��E�B���h�E
     * @param mousePos �}�E�X�ʒu�i�X�N���[�����W�j
     * @param view CanvasView�C���X�^���X
     * @param brushSize �u���V�T�C�Y
     * @param tileSize �^�C���T�C�Y
     */
    void drawCursor(sf::RenderWindow& window,
        const sf::Vector2i& mousePos,
        const CanvasView& view,
        int brushSize,
        int tileSize) const;

    /**
     * �v���r���[�`��
     * �����c�[�����Ńh���b�O���̃v���r���[��\��
     * @param window �`��E�B���h�E
     * @param currentMousePos ���݂̃}�E�X�ʒu�i�X�N���[�����W�j
     * @param view CanvasView�C���X�^���X
     * @param brushSize �u���V�T�C�Y
     */
    void drawPreview(sf::RenderWindow& window,
        const sf::Vector2i& currentMousePos,
        const CanvasView& view,
        int brushSize) const;

    /**
     * �`�擝�v�����擾
     */
    struct DrawingStats {
        bool isDrawing;
        std::string currentTool;
        sf::Vector2i mousePos;
        sf::Vector2i startPos;
        bool hasMoved;
    };

    DrawingStats getDrawingStats() const;

private:
    /**
     * �ړ�����̍X�V
     * �}�E�X���N���b�N���肵�����l�𒴂��Ĉړ��������`�F�b�N
     * @param currentPos ���݂̃}�E�X�ʒu
     */
    void updateMovementFlag(const sf::Vector2i& currentPos);
};

