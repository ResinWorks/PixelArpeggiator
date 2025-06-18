//===== Canvas.cpp CanvasView�Ή������i�����R�[�h�g���j =====
// ������Canvas.cpp�Ɉȉ��̃��\�b�h��ǉ�

#include "Canvas.hpp"
#include "CanvasView.hpp"
#include <iostream>

// �����̃��\�b�h�͂��̂܂ܕێ�...

/**
 * CanvasView�ɑΉ������`�惁�\�b�h
 * ������draw()���\�b�h���x�[�X�ɃY�[���E�p���Ή�
 */
void Canvas::drawWithView(sf::RenderWindow& window,
    const CanvasView& view,
    const std::vector<std::vector<int>>& patterns,
    const std::vector<std::array<sf::Color, 3>>& colorPalettes,
    bool showGrid,
    float spacing,
    float shrink) {

    // RenderTexture�̏�����
    if (!isInitialized) {
        initializeRenderTexture();
    }

    // �ݒ�ύX�̌��o
    bool settingsChanged = (showGrid != lastShowGrid ||
        spacing != lastSpacing ||
        shrink != lastShrink);

    // �_�[�e�B�t���O�܂��͐ݒ�ύX���̍ĕ`��
    if (isDirty || settingsChanged) {
        renderToTexture(patterns, colorPalettes, showGrid, spacing, shrink);
        isDirty = false;
        lastShowGrid = showGrid;
        lastSpacing = spacing;
        lastShrink = shrink;
    }

    // CanvasView�̕ϊ���K�p���ăe�N�X�`����`��
    if (isInitialized) {
        sf::Sprite sprite(renderTexture.getTexture());

        // CanvasView�̕ϊ��s���K�p
        sf::RenderStates states;
        states.transform = view.getTransform();

        // �X�v���C�g�̈ʒu��ݒ�
        sprite.setPosition(position);

        window.draw(sprite, states);

        // ���E����`��
        sf::RectangleShape border(sf::Vector2f(width * tileSize, height * tileSize));
        border.setPosition(position);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1.0f / view.getZoom()); // �Y�[���ɉ��������̑���
        border.setOutlineColor(sf::Color(100, 100, 100));
        window.draw(border, states);
    }
}

/**
 * CanvasView���W�n�ł̃N���b�N����
 */
bool Canvas::containsInView(const CanvasView& view, const sf::Vector2i& screenPos) const {
    // �X�N���[�����W���L�����o�X���W�ɕϊ�
    sf::Vector2i canvasPos = view.screenToCanvas(screenPos);

    // �L�����o�X�̈�����`�F�b�N
    sf::FloatRect canvasRect(position.x, position.y, width * tileSize, height * tileSize);
    return canvasRect.contains(static_cast<sf::Vector2f>(canvasPos));
}

/**
 * CanvasView���W�n�ł̃^�C������
 */
void Canvas::handleClickInView(const CanvasView& view, const sf::Vector2i& screenPos, int patternIndex) {
    if (patternIndex < 0) return;

    // �X�N���[�����W���L�����o�X���W�ɕϊ�
    sf::Vector2i canvasPos = view.screenToCanvas(screenPos);

    // �L�����o�X�̈�����`�F�b�N
    if (!containsInView(view, screenPos)) return;

    // �^�C���C���f�b�N�X���v�Z
    sf::Vector2i tileIndex = screenToTileIndex(view, screenPos);

    if (tileIndex.x >= 0 && tileIndex.x < width &&
        tileIndex.y >= 0 && tileIndex.y < height) {

        // �ύX������ꍇ�̂݃_�[�e�B�t���O��ݒ�
        if (tiles[tileIndex.y][tileIndex.x] != patternIndex) {
            tiles[tileIndex.y][tileIndex.x] = patternIndex;
            isDirty = true;
        }
    }
}

/**
 * CanvasView���W�n�ł̃^�C������
 */
void Canvas::eraseTileInView(const CanvasView& view, const sf::Vector2i& screenPos) {
    // �X�N���[�����W���L�����o�X���W�ɕϊ�
    if (!containsInView(view, screenPos)) return;

    // �^�C���C���f�b�N�X���v�Z
    sf::Vector2i tileIndex = screenToTileIndex(view, screenPos);

    if (tileIndex.x >= 0 && tileIndex.x < width &&
        tileIndex.y >= 0 && tileIndex.y < height) {

        // ���ɋ�̏ꍇ�͉������Ȃ�
        if (tiles[tileIndex.y][tileIndex.x] != -1) {
            tiles[tileIndex.y][tileIndex.x] = -1;
            isDirty = true;
        }
    }
}

/**
 * �p�t�H�[�}���X���擾
 */
Canvas::PerformanceInfo Canvas::getPerformanceInfo(const CanvasView& view) const {
    PerformanceInfo info;
    info.totalTiles = width * height;
    info.zoomLevel = view.getZoom();

    // �\���͈͓��̃^�C�������v�Z
    sf::FloatRect visibleArea = view.getVisibleCanvasArea();

    // �L�����o�X���W�n�ł̕\���͈͂��v�Z
    int startX = std::max(0, static_cast<int>((visibleArea.left - position.x) / tileSize));
    int startY = std::max(0, static_cast<int>((visibleArea.top - position.y) / tileSize));
    int endX = std::min(width, static_cast<int>((visibleArea.left + visibleArea.width - position.x) / tileSize) + 1);
    int endY = std::min(height, static_cast<int>((visibleArea.top + visibleArea.height - position.y) / tileSize) + 1);

    info.visibleTiles = (endX - startX) * (endY - startY);

    // ���ۂɕ`�悳���^�C�������J�E���g
    info.drawnTiles = 0;
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height && tiles[y][x] >= 0) {
                info.drawnTiles++;
            }
        }
    }

    return info;
}

/**
 * CanvasView�p�̍��W�ϊ��w���p�[
 */
sf::Vector2i Canvas::screenToTileIndex(const CanvasView& view, const sf::Vector2i& screenPos) const {
    // �X�N���[�����W���L�����o�X���W�ɕϊ�
    sf::Vector2i canvasPos = view.screenToCanvas(screenPos);

    // �L�����o�X���W����^�C���C���f�b�N�X���v�Z
    sf::Vector2f localPos = static_cast<sf::Vector2f>(canvasPos) - position;
    int tileX = static_cast<int>(localPos.x) / tileSize;
    int tileY = static_cast<int>(localPos.y) / tileSize;

    return sf::Vector2i(tileX, tileY);
}