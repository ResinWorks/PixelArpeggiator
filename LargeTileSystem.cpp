//===== LargeTileSystem.cpp ���S�Łi��]�@�\�Ή��j =====
#include "LargeTileSystem.hpp"
#include "Canvas.hpp"
#include "CanvasView.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ===== �O���[�o���ϐ��̒�`�i�����N�G���[�����j =====
LargeTileManager largeTileManager;

// ===== LargeTileTool ���� =====

/**
 * ��^�^�C���c�[���F�`��J�n
 * �N���b�N�ʒu�ɑ����ɑ�^�^�C����z�u
 */
void LargeTileTool::onDrawStart(const sf::Vector2i& startPos,
    Canvas& canvas,
    const CanvasView& view,
    int patternIndex,
    int brushSize)
{
    isDrawing = true;
    startDrawPos = startPos;
    lastPlacedPos = startPos;

    // ���ۂ̃L�����o�X�^�C���T�C�Y���擾
    int canvasTileSize = canvas.getTileSize();
    int screenTileSize = static_cast<int>(canvasTileSize * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(startPos, view, screenTileSize, brushSize, canvasTileSize);

    // ��^�^�C����z�u
    placeLargeTiles(snappedPos, brushSize, canvas, view);
}

void LargeTileTool::onDrawContinue(const sf::Vector2i& currentPos,
    const sf::Vector2i& lastPos,
    Canvas& canvas,
    const CanvasView& view,
    int patternIndex,
    int brushSize)
{
    if (!isDrawing) return;

    // ���ۂ̃L�����o�X�^�C���T�C�Y���擾
    int canvasTileSize = canvas.getTileSize();
    int screenTileSize = static_cast<int>(canvasTileSize * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(currentPos, view, screenTileSize, brushSize, canvasTileSize);

    // �Ō�ɔz�u�����ʒu�Ɠ����Ȃ�X�L�b�v
    if (snappedPos == lastPlacedPos) {
        return;
    }

    // ��^�^�C����z�u
    placeLargeTiles(snappedPos, brushSize, canvas, view);
    lastPlacedPos = snappedPos;
}

void LargeTileTool::onDrawEnd(const sf::Vector2i& endPos,
    const sf::Vector2i& startPos,
    Canvas& canvas,
    const CanvasView& view,
    int patternIndex,
    int brushSize)
{
    isDrawing = false;
}

/**
 * �v���r���[�`��i��]�Ή��j
 */
void LargeTileTool::drawPreview(sf::RenderWindow& window,
    const sf::Vector2i& startPos,
    const sf::Vector2i& currentPos,
    const CanvasView& view,
    int brushSize) const
{
    // ���݂̑�^�^�C���̉�]��Ԃ��擾
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();
    RotationAngle rotation = currentTile.getCurrentRotation();

    int estimatedTileSize = 6; // �b��l
    sf::Vector2i snappedPos = snapPosition(currentPos, view, estimatedTileSize, brushSize, estimatedTileSize);

    // ��]���l�������T�C�Y�v�Z
    int baseWidth, baseHeight;
    if (currentLargeTileId >= 8 && currentLargeTileId <= 11) {
        // 4x2�^�C��
        if (rotation == RotationAngle::ROTATE_90 || rotation == RotationAngle::ROTATE_270) {
            baseWidth = 2; baseHeight = 4; // 90�x/270�x��]��2x4�ɂȂ�
        }
        else {
            baseWidth = 4; baseHeight = 2; // 0�x/180�x��4x2�̂܂�
        }
    }
    else {
        // 2x2�^�C���i��]���Ă�2x2�̂܂܁j
        baseWidth = 2; baseHeight = 2;
    }

    // �P��^�^�C���̃T�C�Y
    int tileWidth = baseWidth * estimatedTileSize;
    int tileHeight = baseHeight * estimatedTileSize;

    // �u���V�T�C�Y�ɉ������S�̃T�C�Y
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // �v���r���[��`�`��i�������j
    sf::RectangleShape preview(sf::Vector2f(totalWidth, totalHeight));
    preview.setPosition(static_cast<sf::Vector2f>(snappedPos) - sf::Vector2f(totalWidth / 2, totalHeight / 2));
    preview.setFillColor(sf::Color(255, 255, 255, 30)); // ��蔖��������

    // ��]��Ԃɉ������g�F
    sf::Color frameColor = sf::Color(255, 255, 0, 100); // �f�t�H���g�F���F
    switch (rotation) {
    case RotationAngle::ROTATE_90:  frameColor = sf::Color(0, 255, 255, 100); break;  // �V�A��
    case RotationAngle::ROTATE_180: frameColor = sf::Color(255, 0, 255, 100); break;  // �}�[���^
    case RotationAngle::ROTATE_270: frameColor = sf::Color(255, 165, 0, 100); break;  // �I�����W
    default: break;
    }

    preview.setOutlineThickness(2.0f);
    preview.setOutlineColor(frameColor);
    window.draw(preview);

    // ��^�^�C���ԍ��\��
    sf::CircleShape marker(8.0f);
    marker.setOrigin(8.0f, 8.0f);
    marker.setPosition(static_cast<sf::Vector2f>(snappedPos));
    marker.setFillColor(sf::Color(255, 0, 0, 150));
    window.draw(marker);
}

/**
 * �J�[�\���`��i��]�Ή��j
 */
void LargeTileTool::drawCursor(sf::RenderWindow& window,
    const sf::Vector2i& mousePos,
    const CanvasView& view,
    int brushSize,
    int tileSize) const
{
    float zoom = view.getZoom();
    float scaledTileSize = tileSize * zoom;

    // ���݂̑�^�^�C���̉�]��Ԃ��擾
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();
    RotationAngle rotation = currentTile.getCurrentRotation();

    // ��{�̑�^�^�C���T�C�Y�i��]���l���j
    int baseWidth, baseHeight;
    if (currentLargeTileId >= 8 && currentLargeTileId <= 11) {
        // 4x2�^�C��
        if (rotation == RotationAngle::ROTATE_90 || rotation == RotationAngle::ROTATE_270) {
            baseWidth = 2; baseHeight = 4; // 90�x/270�x��]��2x4�ɂȂ�
        }
        else {
            baseWidth = 4; baseHeight = 2; // 0�x/180�x��4x2�̂܂�
        }
    }
    else {
        // 2x2�^�C���i��]���Ă�2x2�̂܂܁j
        baseWidth = 2; baseHeight = 2;
    }

    // �P��^�^�C���̃T�C�Y
    float tileWidth = baseWidth * scaledTileSize;
    float tileHeight = baseHeight * scaledTileSize;

    // �u���V�T�C�Y�ɉ������S�̃T�C�Y
    float totalWidth = tileWidth * brushSize;
    float totalHeight = tileHeight * brushSize;

    // �X�i�b�v�ʒu���v�Z
    int screenTileSize = static_cast<int>(tileSize * zoom);
    sf::Vector2i snappedPos = snapPosition(mousePos, view, screenTileSize, brushSize, tileSize);

    // ��^�^�C���g�`��
    sf::RectangleShape largeTileFrame(sf::Vector2f(totalWidth, totalHeight));
    largeTileFrame.setPosition(static_cast<sf::Vector2f>(snappedPos) - sf::Vector2f(totalWidth / 2, totalHeight / 2));
    largeTileFrame.setFillColor(sf::Color::Transparent);
    largeTileFrame.setOutlineThickness(2.0f);

    // ��]��Ԃɉ����Ęg�̐F��ύX
    sf::Color frameColor = sf::Color(255, 255, 0, 200); // �f�t�H���g�F���F
    switch (rotation) {
    case RotationAngle::ROTATE_90:  frameColor = sf::Color(0, 255, 255, 200); break;  // �V�A��
    case RotationAngle::ROTATE_180: frameColor = sf::Color(255, 0, 255, 200); break;  // �}�[���^
    case RotationAngle::ROTATE_270: frameColor = sf::Color(255, 165, 0, 200); break;  // �I�����W
    default: break;
    }
    largeTileFrame.setOutlineColor(frameColor);
    window.draw(largeTileFrame);

    // �����O���b�h���`��
    sf::Color boundaryColor(255, 255, 255, 150);
    float lineThickness = 1.0f;

    // �c���i�u���V�T�C�Y�P�ʁj
    for (int i = 1; i < brushSize; i++) {
        float xPos = largeTileFrame.getPosition().x + i * tileWidth;
        sf::RectangleShape vLine(sf::Vector2f(lineThickness, totalHeight));
        vLine.setPosition(xPos, largeTileFrame.getPosition().y);
        vLine.setFillColor(boundaryColor);
        window.draw(vLine);
    }

    // �����i�u���V�T�C�Y�P�ʁj
    for (int i = 1; i < brushSize; i++) {
        float yPos = largeTileFrame.getPosition().y + i * tileHeight;
        sf::RectangleShape hLine(sf::Vector2f(totalWidth, lineThickness));
        hLine.setPosition(largeTileFrame.getPosition().x, yPos);
        hLine.setFillColor(boundaryColor);
        window.draw(hLine);
    }

    // ��^�^�C���ԍ��{��]�p�x�\��
    sf::CircleShape idMarker(8.0f);
    idMarker.setOrigin(8.0f, 8.0f);
    idMarker.setPosition(static_cast<sf::Vector2f>(snappedPos));
    idMarker.setFillColor(frameColor);
    idMarker.setOutlineThickness(1.0f);
    idMarker.setOutlineColor(sf::Color::White);
    window.draw(idMarker);

    // ��]�p�x�C���W�P�[�^�[�i�����̏����Ȗ��j
    if (rotation != RotationAngle::ROTATE_0) {
        drawRotationIndicator(window, snappedPos, rotation);
    }
}

/**
 * �ʒu���^�^�C���T�C�Y�ƃu���V�T�C�Y�ɍ��킹�Đ��m�ɃX�i�b�v
 */
sf::Vector2i LargeTileTool::snapPosition(const sf::Vector2i& pos,
    const CanvasView& view,
    int screenTileSize,
    int brushSize,
    int canvasTileSize) const
{
    // ���݂̑�^�^�C���̉�]��Ԃ��擾
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();
    RotationAngle rotation = currentTile.getCurrentRotation();

    // ��^�^�C���̊�{�T�C�Y�i��]�l���j
    int baseWidth, baseHeight;
    if (currentLargeTileId >= 8 && currentLargeTileId <= 11) {
        // 4x2�^�C��
        if (rotation == RotationAngle::ROTATE_90 || rotation == RotationAngle::ROTATE_270) {
            baseWidth = 2; baseHeight = 4;
        }
        else {
            baseWidth = 4; baseHeight = 2;
        }
    }
    else {
        // 2x2�^�C��
        baseWidth = 2; baseHeight = 2;
    }

    // �P��^�^�C���̃T�C�Y�i�X�N���[�����W�j
    int tileWidth = screenTileSize * baseWidth;
    int tileHeight = screenTileSize * baseHeight;

    // �u���V�S�̂̃T�C�Y
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // ��萳�m�ȃX�i�b�v�v�Z
    int halfTotalWidth = totalWidth / 2;
    int halfTotalHeight = totalHeight / 2;

    // �X�i�b�v�ʒu���v�Z�i���S�_����Ɂj
    int snappedX = ((pos.x - halfTotalWidth) / tileWidth) * tileWidth + halfTotalWidth;
    int snappedY = ((pos.y - halfTotalHeight) / tileHeight) * tileHeight + halfTotalHeight;

    return sf::Vector2i(snappedX, snappedY);
}

/**
 * �u���V�T�C�Y�ɉ����đ�^�^�C���𕡐��z�u
 */
void LargeTileTool::placeLargeTiles(const sf::Vector2i& basePos,
    int brushSize,
    Canvas& canvas,
    const CanvasView& view) const
{
    // ���݂̑�^�^�C���̉�]��Ԃ��擾
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();
    RotationAngle rotation = currentTile.getCurrentRotation();

    // ��{�̑�^�^�C���T�C�Y�i��]�l���j
    int baseWidth, baseHeight;
    if (currentLargeTileId >= 8 && currentLargeTileId <= 11) {
        // 4x2�^�C��
        if (rotation == RotationAngle::ROTATE_90 || rotation == RotationAngle::ROTATE_270) {
            baseWidth = 2; baseHeight = 4;
        }
        else {
            baseWidth = 4; baseHeight = 2;
        }
    }
    else {
        // 2x2�^�C��
        baseWidth = 2; baseHeight = 2;
    }

    // ���ۂ̃L�����o�X�^�C���T�C�Y���g�p
    int canvasTileSize = canvas.getTileSize();
    int screenTileSize = static_cast<int>(canvasTileSize * view.getZoom());

    // �P��^�^�C���̃T�C�Y�i�X�N���[�����W�j
    int tileWidth = baseWidth * screenTileSize;
    int tileHeight = baseHeight * screenTileSize;

    // �u���V�T�C�Y�ɉ������I�t�Z�b�g�͈�
    int offsetRange = (brushSize - 1) / 2;

    // �I�t�Z�b�g�������ĕ����̑�^�^�C����z�u
    for (int dx = -offsetRange; dx <= offsetRange; dx++) {
        for (int dy = -offsetRange; dy <= offsetRange; dy++) {
            // �z�u�ʒu���v�Z�i�X�N���[�����W�j
            sf::Vector2i tileScreenPos(
                basePos.x + dx * tileWidth,
                basePos.y + dy * tileHeight
            );

            // �L�����o�X���W�ɕϊ�
            sf::Vector2i tileCanvasPos = view.screenToCanvas(tileScreenPos);

            // �P�̑�^�^�C����z�u
            placeLargeTile(tileCanvasPos, canvas, view);
        }
    }
}

/**
 * �P�̑�^�^�C����z�u
 */
void LargeTileTool::placeLargeTile(const sf::Vector2i& canvasPos,
    Canvas& canvas,
    const CanvasView& view) const
{
    // �O���[�o����LargeTileManager���猻�݂̑�^�^�C�����擾
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();

    auto arrangement = currentTile.getArrangement(); // ��]���K�p���ꂽ�z�u���擾
    auto positions = currentTile.getDrawPositions(canvasPos, canvas.getTileSize());

    for (size_t i = 0; i < arrangement.indices.size(); ++i) {
        if (i < positions.size()) {
            sf::Vector2i screenPos = view.canvasToScreen(positions[i]);
            if (canvas.containsInView(view, screenPos)) {
                canvas.handleClickInView(
                    view,
                    screenPos,
                    arrangement.indices[i]
                );
            }
        }
    }
}

/**
 * ��]�C���W�P�[�^�[��`��
 */
void LargeTileTool::drawRotationIndicator(sf::RenderWindow& window,
    const sf::Vector2i& center,
    RotationAngle rotation) const
{
    float radius = 6.0f;
    float angleRad = static_cast<float>(rotation) * M_PI / 180.0f;

    sf::Vector2f arrowStart = static_cast<sf::Vector2f>(center);
    sf::Vector2f arrowEnd(
        center.x + radius * cos(angleRad),
        center.y + radius * sin(angleRad)
    );

    // ����
    sf::VertexArray arrow(sf::Lines, 2);
    arrow[0] = sf::Vertex(arrowStart, sf::Color::White);
    arrow[1] = sf::Vertex(arrowEnd, sf::Color::White);
    window.draw(arrow);

    // ���̐�[
    sf::CircleShape arrowHead(1.5f);
    arrowHead.setOrigin(1.5f, 1.5f);
    arrowHead.setPosition(arrowEnd);
    arrowHead.setFillColor(sf::Color::White);
    window.draw(arrowHead);
}