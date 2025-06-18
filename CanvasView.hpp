//===== CanvasView.hpp �Y�[���C���� =====
#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

class CanvasView {
private:
    float zoomLevel;
    sf::Vector2f panOffset;
    sf::Vector2f basePosition;

    static constexpr float MIN_ZOOM = 0.1f;
    static constexpr float MAX_ZOOM = 10.0f;
    static constexpr float ZOOM_STEP = 1.2f;

    sf::Vector2u windowSize;

public:
    CanvasView(const sf::Vector2f& basePos, const sf::Vector2u& winSize)
        : zoomLevel(2.0f)
        , panOffset(0.0f, 0.0f)
        , basePosition(basePos)
        , windowSize(winSize) {}

    // ===== �Y�[������i�C���Łj =====

    /**
     * �}�E�X�ʒu�𒆐S�ɃY�[���C��
     */
    void zoomIn(const sf::Vector2i& mousePos) {
        zoomAt(mousePos, ZOOM_STEP);
    }

    /**
     * �}�E�X�ʒu�𒆐S�ɃY�[���A�E�g
     */
    void zoomOut(const sf::Vector2i& mousePos) {
        zoomAt(mousePos, 1.0f / ZOOM_STEP);
    }

    /**
     * �Y�[�����x���𒼐ڐݒ�
     */
    void setZoom(float zoom) {
        zoomLevel = std::clamp(zoom, MIN_ZOOM, MAX_ZOOM);
    }

    float getZoom() const { return zoomLevel; }

    // ===== �p������ =====

    void pan(const sf::Vector2f& delta) {
        panOffset += delta;
    }

    void setPanOffset(const sf::Vector2f& offset) {
        panOffset = offset;
    }

    sf::Vector2f getPanOffset() const { return panOffset; }

    // ===== ���W�ϊ��i�C���Łj =====

    /**
     * �X�N���[�����W���L�����o�X���W�ɕϊ��i�C���Łj
     */
    sf::Vector2i screenToCanvas(const sf::Vector2i& screenPos) const {
        // �X�N���[�����W���L�����o�X���W�ɕϊ�
        sf::Vector2f screenFloat = static_cast<sf::Vector2f>(screenPos);

        // �p���I�t�Z�b�g������
        sf::Vector2f withoutPan = screenFloat - panOffset;

        // ��ʒu����̑��Έʒu���v�Z
        sf::Vector2f relative = withoutPan - basePosition;

        // �Y�[�����������Č��̃L�����o�X���W���擾
        sf::Vector2f canvasFloat = relative / zoomLevel + basePosition;

        return static_cast<sf::Vector2i>(canvasFloat);
    }

    /**
     * �L�����o�X���W���X�N���[�����W�ɕϊ��i�C���Łj
     */
    sf::Vector2i canvasToScreen(const sf::Vector2i& canvasPos) const {
        sf::Vector2f canvasFloat = static_cast<sf::Vector2f>(canvasPos);

        // ��ʒu����̑��Έʒu���v�Z
        sf::Vector2f relative = canvasFloat - basePosition;

        // �Y�[����K�p
        sf::Vector2f zoomed = relative * zoomLevel;

        // ��ʒu�ƃp���I�t�Z�b�g��K�p
        sf::Vector2f screenFloat = zoomed + basePosition + panOffset;

        return static_cast<sf::Vector2i>(screenFloat);
    }

    // ===== �`��p�ϊ��s��i�C���Łj =====

    sf::Transform getTransform() const {
        sf::Transform transform;

        // 1. ��ʒu�Ɉړ�
        transform.translate(basePosition);

        // 2. �p���I�t�Z�b�g��K�p
        transform.translate(panOffset);

        // 3. �Y�[����K�p
        transform.scale(zoomLevel, zoomLevel);

        // 4. ��ʒu�����_�ɖ߂�
        transform.translate(-basePosition);

        return transform;
    }

    // ===== �\������ =====

    bool isVisible(const sf::FloatRect& canvasRect) const {
        sf::FloatRect visibleArea = getVisibleCanvasArea();
        return visibleArea.intersects(canvasRect);
    }

    sf::FloatRect getVisibleCanvasArea() const {
        sf::Vector2i topLeft = screenToCanvas(sf::Vector2i(0, 0));
        sf::Vector2i bottomRight = screenToCanvas(static_cast<sf::Vector2i>(windowSize));

        return sf::FloatRect(
            static_cast<float>(topLeft.x),
            static_cast<float>(topLeft.y),
            static_cast<float>(bottomRight.x - topLeft.x),
            static_cast<float>(bottomRight.y - topLeft.y)
        );
    }

    // ===== �ݒ�X�V =====

    void updateWindowSize(const sf::Vector2u& newSize) {
        windowSize = newSize;
    }

    void setBasePosition(const sf::Vector2f& newBasePos) {
        basePosition = newBasePos;
    }

    // ===== �r���[�̃��Z�b�g =====

    /**
     * �r���[��Ԃ����Z�b�g�i�Y�[��1.0�A�p���I�t�Z�b�g0�j
     */
    void reset() {
        zoomLevel = 1.0f;
        panOffset = sf::Vector2f(0.0f, 0.0f);
    }

    /**
     * �}�E�X�z�C�[���ɂ��Y�[������i�C���Łj
     */
    void handleMouseWheel(float wheelDelta, const sf::Vector2i& mousePos) {
        if (wheelDelta > 0) {
            zoomIn(mousePos);
        }
        else if (wheelDelta < 0) {
            zoomOut(mousePos);
        }
    }

    /**
     * ���݂̃r���[��Ԃ𕶎���Ƃ��Ď擾
     */
    std::string getStatusString() const {
        std::ostringstream oss;
        oss << "Zoom: " << std::fixed << std::setprecision(1) << (zoomLevel * 100.0f) << "%";
        oss << " | Pan: (" << static_cast<int>(panOffset.x) << ", " << static_cast<int>(panOffset.y) << ")";
        return oss.str();
    }

private:
    /**
     * �w����W�𒆐S�ɃY�[�����s�i�C���Łj
     */
    void zoomAt(const sf::Vector2i& centerPos, float factor) {
        float oldZoom = zoomLevel;
        float newZoom = std::clamp(zoomLevel * factor, MIN_ZOOM, MAX_ZOOM);

        if (std::abs(newZoom - oldZoom) < 0.001f) return;

        // �Y�[���O�̃}�E�X�ʒu�ł̃L�����o�X���W���擾
        sf::Vector2i canvasPosBeforeZoom = screenToCanvas(centerPos);

        // �Y�[�����x���X�V
        zoomLevel = newZoom;

        // �Y�[����̓����L�����o�X���W�̃X�N���[���ʒu���v�Z
        sf::Vector2i screenPosAfterZoom = canvasToScreen(canvasPosBeforeZoom);

        // �}�E�X�ʒu���ς��Ȃ��悤�Ƀp���I�t�Z�b�g�𒲐�
        sf::Vector2f correction = static_cast<sf::Vector2f>(centerPos - screenPosAfterZoom);
        panOffset += correction;
    }
};