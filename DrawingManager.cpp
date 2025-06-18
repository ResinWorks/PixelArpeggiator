//===== DrawingManager.cpp =====
#include "DrawingManager.hpp"
#include "Canvas.hpp"
#include "CanvasView.hpp"

/**
 * �`��J�n
 * �I�����ꂽ�c�[���̕`��J�n�������Ăяo��
 */
void DrawingManager::startDrawing(const sf::Vector2i& startPos,
    Canvas& canvas,
    const CanvasView& view,
    int patternIndex,
    int brushSize) {
    isDrawing = true;
    lastMousePos = startPos;
    mouseDownPos = startPos;
    hasMoved = false;

    // ���݂̃c�[���ɕ`��J�n��ʒm
    DrawingTool* tool = toolManager.getCurrentTool();
    if (tool) {
        tool->onDrawStart(startPos, canvas, view, patternIndex, brushSize);
    }
}

/**
 * �`��I��
 * �I�����ꂽ�c�[���̕`��I���������Ăяo��
 */
void DrawingManager::stopDrawing(const sf::Vector2i& endPos,
    Canvas& canvas,
    const CanvasView& view,
    int patternIndex,
    int brushSize) {
    if (!isDrawing) return;

    DrawingTool* tool = toolManager.getCurrentTool();
    if (tool) {
        // �P���N���b�N�i�ړ��Ȃ��j�̏ꍇ�̓��ʏ���
        if (!hasMoved) {
            // �A���`����T�|�[�g���Ȃ��c�[���i�������j�ł͉������Ȃ�
            // �A���`��c�[���i�u���V���j�ł͊J�n���Ɋ��ɕ`��ς�
            if (!tool->supportsContinuousDrawing()) {
                // �������ł͊J�n�ʒu�ƏI���ʒu�������ꍇ�͓_��`��
                tool->onDrawEnd(endPos, mouseDownPos, canvas, view, patternIndex, brushSize);
            }
        }
        else {
            // �ړ�����̏ꍇ�͏I�����������s
            tool->onDrawEnd(endPos, mouseDownPos, canvas, view, patternIndex, brushSize);
        }
    }

    isDrawing = false;
}

/**
 * �}�E�X�ړ��̍X�V
 * �A���`��c�[���̏ꍇ�͕`��p���������Ăяo��
 */
void DrawingManager::updateMovement(const sf::Vector2i& currentPos,
    Canvas& canvas,
    const CanvasView& view,
    int patternIndex,
    int brushSize) {
    // �ړ�������X�V
    updateMovementFlag(currentPos);

    // �`�撆���c�[�����A���`����T�|�[�g����ꍇ
    if (isDrawing && hasMoved) {
        DrawingTool* tool = toolManager.getCurrentTool();
        if (tool && tool->supportsContinuousDrawing()) {
            // �A���`�揈�������s
            tool->onDrawContinue(currentPos, lastMousePos, canvas, view, patternIndex, brushSize);
            lastMousePos = currentPos;
        }
    }
}

/**
 * �J�[�\���`��
 * ���݂̃c�[���ɉ������J�[�\����\��
 */
void DrawingManager::drawCursor(sf::RenderWindow& window,
    const sf::Vector2i& mousePos,
    const CanvasView& view,
    int brushSize,
    int tileSize) const {
    DrawingTool* tool = toolManager.getCurrentTool();
    if (tool) {
        tool->drawCursor(window, mousePos, view, brushSize, tileSize);
    }
}

/**
 * �v���r���[�`��
 * �����c�[�����Ńh���b�O���̃v���r���[��\��
 */
void DrawingManager::drawPreview(sf::RenderWindow& window,
    const sf::Vector2i& currentMousePos,
    const CanvasView& view,
    int brushSize) const {
    // �`�撆���ړ�����̏ꍇ�̂݃v���r���[�\��
    if (isDrawing && hasMoved) {
        DrawingTool* tool = toolManager.getCurrentTool();
        if (tool) {
            tool->drawPreview(window, mouseDownPos, currentMousePos, view, brushSize);
        }
    }
}

/**
 * �`�擝�v�����擾
 */
DrawingManager::DrawingStats DrawingManager::getDrawingStats() const {
    return {
        isDrawing,
        toolManager.getCurrentToolName(),
        lastMousePos,
        mouseDownPos,
        hasMoved
    };
}

/**
 * �ړ�����̍X�V
 * �}�E�X���N���b�N���肵�����l�𒴂��Ĉړ��������`�F�b�N
 */
void DrawingManager::updateMovementFlag(const sf::Vector2i& currentPos) {
    if (isDrawing && !hasMoved) {
        int deltaX = std::abs(currentPos.x - mouseDownPos.x);
        int deltaY = std::abs(currentPos.y - mouseDownPos.y);
        if (deltaX > CLICK_THRESHOLD || deltaY > CLICK_THRESHOLD) {
            hasMoved = true;
        }
    }
}