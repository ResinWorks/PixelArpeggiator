//===== LargeTileSystem.cpp ��^�^�C���c�[������ =====
#include "LargeTileSystem.hpp"
#include "Canvas.hpp"
#include "CanvasView.hpp"

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

    // �X�i�b�v�ʒu���v�Z
    int tileSize = static_cast<int>(6 * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(startPos, view, tileSize, brushSize);

    // ��^�^�C����z�u
  // placeLargeTiles(startPos, brushSize, canvas, view);
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

    int tileSize = static_cast<int>(6 * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(currentPos, view, tileSize, brushSize);

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
 * �v���r���[�`��
 */
void LargeTileTool::drawPreview(sf::RenderWindow& window,
    const sf::Vector2i& startPos,
    const sf::Vector2i& currentPos,
    const CanvasView& view,
    int brushSize) const
{
    int tileSize = static_cast<int>(6 * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(currentPos, view, tileSize, brushSize);

    // ��{�̑�^�^�C���T�C�Y
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // �P���^�^�C���̃T�C�Y
    int tileWidth = baseWidth * tileSize;
    int tileHeight = baseHeight * tileSize;

    // �u���V�T�C�Y�ɉ������S�̃T�C�Y
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // �v���r���[��`�`��
    sf::RectangleShape preview(sf::Vector2f(totalWidth, totalHeight));
    preview.setPosition(static_cast<sf::Vector2f>(snappedPos) - sf::Vector2f(totalWidth / 2, totalHeight / 2));
    preview.setFillColor(sf::Color(255, 255, 255, 50));
    preview.setOutlineThickness(2.0f);
    preview.setOutlineColor(sf::Color(255, 255, 0, 150));
    window.draw(preview);

    // ��^�^�C���ԍ��\��
    sf::CircleShape marker(8.0f);
    marker.setOrigin(8.0f, 8.0f);
    marker.setPosition(static_cast<sf::Vector2f>(snappedPos));
    marker.setFillColor(sf::Color(255, 0, 0, 150));
    window.draw(marker);
}

/**
 * �J�[�\���`��
 */
void LargeTileTool::drawCursor(sf::RenderWindow& window,
    const sf::Vector2i& mousePos,
    const CanvasView& view,
    int brushSize,
    int tileSize) const
{
    float zoom = view.getZoom();
    float scaledTileSize = tileSize * zoom;

    // ��{�̑�^�^�C���T�C�Y
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // �P���^�^�C���̃T�C�Y
    float tileWidth = baseWidth * scaledTileSize;
    float tileHeight = baseHeight * scaledTileSize;

    // �u���V�T�C�Y�ɉ������S�̃T�C�Y
    float totalWidth = tileWidth * brushSize;
    float totalHeight = tileHeight * brushSize;

    // �X�i�b�v�ʒu���v�Z
    int screenTileSize = static_cast<int>(6 * zoom);
    sf::Vector2i snappedPos = snapPosition(mousePos, view, screenTileSize, brushSize);

    // ��^�^�C���g�`��
    sf::RectangleShape largeTileFrame(
        sf::Vector2f(totalWidth, totalHeight)
    );
    largeTileFrame.setPosition(static_cast<sf::Vector2f>(snappedPos) - sf::Vector2f(totalWidth / 2, totalHeight / 2));
    largeTileFrame.setFillColor(sf::Color::Transparent);
    largeTileFrame.setOutlineThickness(2.0f);
    largeTileFrame.setOutlineColor(sf::Color(255, 255, 0, 200));
    window.draw(largeTileFrame);

    // �����O���b�h���`��
    sf::Color boundaryColor(255, 255, 255, 150);
    float lineThickness = 1.0f;

    // �c���i�u���V�T�C�Y�P�ʁj
    for (int i = 1; i < brushSize; i++) {
        float xPos = largeTileFrame.getPosition().x + i * tileWidth;
        sf::RectangleShape vLine(sf::Vector2f(
            lineThickness,
            totalHeight
        ));
        vLine.setPosition(xPos, largeTileFrame.getPosition().y);
        vLine.setFillColor(boundaryColor);
        window.draw(vLine);
    }

    // �����i�u���V�T�C�Y�P�ʁj
    for (int i = 1; i < brushSize; i++) {
        float yPos = largeTileFrame.getPosition().y + i * tileHeight;
        sf::RectangleShape hLine(sf::Vector2f(
            totalWidth,
            lineThickness
        ));
        hLine.setPosition(largeTileFrame.getPosition().x, yPos);
        hLine.setFillColor(boundaryColor);
        window.draw(hLine);
    }

    // ��^�^�C���ԍ��\��
    sf::CircleShape idMarker(6.0f);
    idMarker.setOrigin(6.0f, 6.0f);
    idMarker.setPosition(static_cast<sf::Vector2f>(snappedPos));
    idMarker.setFillColor(sf::Color(255, 0, 0, 180));
    idMarker.setOutlineThickness(1.0f);
    idMarker.setOutlineColor(sf::Color::White);
    window.draw(idMarker);
}
/**
 * �ʒu���^�^�C���T�C�Y�ƃu���V�T�C�Y�ɍ��킹�Đ��m�ɃX�i�b�v
 */
sf::Vector2i LargeTileTool::snapPosition(const sf::Vector2i& pos,
    const CanvasView& view,
    int tileSize,
    int brushSize) const
{
    // ��^�^�C���̊�{�T�C�Y
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // �P���^�^�C���̃T�C�Y
    int tileWidth = tileSize * baseWidth;
    int tileHeight = tileSize * baseHeight;

    // �u���V�S�̂̃T�C�Y
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // �X�i�b�v�ʒu���v�Z�i���S�_����Ɂj
    int snappedX = ((pos.x - totalWidth / 2) / tileWidth) * tileWidth;
    int snappedY = ((pos.y - totalHeight / 2) / tileHeight) * tileHeight;

    // �ʒu�𒆐S�ɒ���
    snappedX += totalWidth / 2;
    snappedY += totalHeight / 2;

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
    // ��{�̑�^�^�C���T�C�Y
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;
    
    // �X�N���[����̃^�C���T�C�Y
    int screenTileSize = static_cast<int>(6 * view.getZoom());
    // �L�����o�X��̃^�C���T�C�Y
    int canvasTileSize = canvas.getTileSize();
    
    // �P���^�^�C���̃T�C�Y
    int tileWidth = baseWidth * screenTileSize;
    int tileHeight = baseHeight * screenTileSize;
    
    // �u���V�T�C�Y�ɉ������I�t�Z�b�g�͈�
    int offsetRange = (brushSize - 1) / 2;
    
    // ���S�ʒu�i�L�����o�X���W�j
    sf::Vector2i centerCanvasPos = view.screenToCanvas(basePos);
    
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
            
            // �P��̑�^�^�C����z�u
            placeLargeTile(tileCanvasPos, canvas, view);
        }
    }
}
/**
 * �P��̑�^�^�C����z�u
 */
void LargeTileTool::placeLargeTile(const sf::Vector2i& canvasPos,
    Canvas& canvas,
    const CanvasView& view) const
{
    LargeTile largeTile(currentLargeTileId);
    auto arrangement = largeTile.getArrangement();
    auto positions = largeTile.getDrawPositions(
        canvasPos,
        canvas.getTileSize()
    );

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