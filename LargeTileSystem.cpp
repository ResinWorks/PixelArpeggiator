//===== LargeTileSystem.cpp 大型タイルツール実装 =====
#include "LargeTileSystem.hpp"
#include "Canvas.hpp"
#include "CanvasView.hpp"

// ===== LargeTileTool 実装 =====

/**
 * 大型タイルツール：描画開始
 * クリック位置に即座に大型タイルを配置
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

    // スナップ位置を計算
    int tileSize = static_cast<int>(6 * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(startPos, view, tileSize, brushSize);

    // 大型タイルを配置
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

    // 最後に配置した位置と同じならスキップ
    if (snappedPos == lastPlacedPos) {
        return;
    }

    // 大型タイルを配置
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
 * プレビュー描画
 */
void LargeTileTool::drawPreview(sf::RenderWindow& window,
    const sf::Vector2i& startPos,
    const sf::Vector2i& currentPos,
    const CanvasView& view,
    int brushSize) const
{
    int tileSize = static_cast<int>(6 * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(currentPos, view, tileSize, brushSize);

    // 基本の大型タイルサイズ
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // 単一大型タイルのサイズ
    int tileWidth = baseWidth * tileSize;
    int tileHeight = baseHeight * tileSize;

    // ブラシサイズに応じた全体サイズ
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // プレビュー矩形描画
    sf::RectangleShape preview(sf::Vector2f(totalWidth, totalHeight));
    preview.setPosition(static_cast<sf::Vector2f>(snappedPos) - sf::Vector2f(totalWidth / 2, totalHeight / 2));
    preview.setFillColor(sf::Color(255, 255, 255, 50));
    preview.setOutlineThickness(2.0f);
    preview.setOutlineColor(sf::Color(255, 255, 0, 150));
    window.draw(preview);

    // 大型タイル番号表示
    sf::CircleShape marker(8.0f);
    marker.setOrigin(8.0f, 8.0f);
    marker.setPosition(static_cast<sf::Vector2f>(snappedPos));
    marker.setFillColor(sf::Color(255, 0, 0, 150));
    window.draw(marker);
}

/**
 * カーソル描画
 */
void LargeTileTool::drawCursor(sf::RenderWindow& window,
    const sf::Vector2i& mousePos,
    const CanvasView& view,
    int brushSize,
    int tileSize) const
{
    float zoom = view.getZoom();
    float scaledTileSize = tileSize * zoom;

    // 基本の大型タイルサイズ
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // 単一大型タイルのサイズ
    float tileWidth = baseWidth * scaledTileSize;
    float tileHeight = baseHeight * scaledTileSize;

    // ブラシサイズに応じた全体サイズ
    float totalWidth = tileWidth * brushSize;
    float totalHeight = tileHeight * brushSize;

    // スナップ位置を計算
    int screenTileSize = static_cast<int>(6 * zoom);
    sf::Vector2i snappedPos = snapPosition(mousePos, view, screenTileSize, brushSize);

    // 大型タイル枠描画
    sf::RectangleShape largeTileFrame(
        sf::Vector2f(totalWidth, totalHeight)
    );
    largeTileFrame.setPosition(static_cast<sf::Vector2f>(snappedPos) - sf::Vector2f(totalWidth / 2, totalHeight / 2));
    largeTileFrame.setFillColor(sf::Color::Transparent);
    largeTileFrame.setOutlineThickness(2.0f);
    largeTileFrame.setOutlineColor(sf::Color(255, 255, 0, 200));
    window.draw(largeTileFrame);

    // 内部グリッド線描画
    sf::Color boundaryColor(255, 255, 255, 150);
    float lineThickness = 1.0f;

    // 縦線（ブラシサイズ単位）
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

    // 横線（ブラシサイズ単位）
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

    // 大型タイル番号表示
    sf::CircleShape idMarker(6.0f);
    idMarker.setOrigin(6.0f, 6.0f);
    idMarker.setPosition(static_cast<sf::Vector2f>(snappedPos));
    idMarker.setFillColor(sf::Color(255, 0, 0, 180));
    idMarker.setOutlineThickness(1.0f);
    idMarker.setOutlineColor(sf::Color::White);
    window.draw(idMarker);
}
/**
 * 位置を大型タイルサイズとブラシサイズに合わせて正確にスナップ
 */
sf::Vector2i LargeTileTool::snapPosition(const sf::Vector2i& pos,
    const CanvasView& view,
    int tileSize,
    int brushSize) const
{
    // 大型タイルの基本サイズ
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // 単一大型タイルのサイズ
    int tileWidth = tileSize * baseWidth;
    int tileHeight = tileSize * baseHeight;

    // ブラシ全体のサイズ
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // スナップ位置を計算（中心点を基準に）
    int snappedX = ((pos.x - totalWidth / 2) / tileWidth) * tileWidth;
    int snappedY = ((pos.y - totalHeight / 2) / tileHeight) * tileHeight;

    // 位置を中心に調整
    snappedX += totalWidth / 2;
    snappedY += totalHeight / 2;

    return sf::Vector2i(snappedX, snappedY);
}
/**
 * ブラシサイズに応じて大型タイルを複数配置
 */
void LargeTileTool::placeLargeTiles(const sf::Vector2i& basePos,
    int brushSize,
    Canvas& canvas,
    const CanvasView& view) const 
{
    // 基本の大型タイルサイズ
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;
    
    // スクリーン上のタイルサイズ
    int screenTileSize = static_cast<int>(6 * view.getZoom());
    // キャンバス上のタイルサイズ
    int canvasTileSize = canvas.getTileSize();
    
    // 単一大型タイルのサイズ
    int tileWidth = baseWidth * screenTileSize;
    int tileHeight = baseHeight * screenTileSize;
    
    // ブラシサイズに応じたオフセット範囲
    int offsetRange = (brushSize - 1) / 2;
    
    // 中心位置（キャンバス座標）
    sf::Vector2i centerCanvasPos = view.screenToCanvas(basePos);
    
    // オフセットをかけて複数の大型タイルを配置
    for (int dx = -offsetRange; dx <= offsetRange; dx++) {
        for (int dy = -offsetRange; dy <= offsetRange; dy++) {
            // 配置位置を計算（スクリーン座標）
            sf::Vector2i tileScreenPos(
                basePos.x + dx * tileWidth,
                basePos.y + dy * tileHeight
            );
            
            // キャンバス座標に変換
            sf::Vector2i tileCanvasPos = view.screenToCanvas(tileScreenPos);
            
            // 単一の大型タイルを配置
            placeLargeTile(tileCanvasPos, canvas, view);
        }
    }
}
/**
 * 単一の大型タイルを配置
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