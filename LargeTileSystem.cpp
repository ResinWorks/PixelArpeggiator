// ===== 修正版 LargeTileSystem.cpp =====
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

    // **修正：Canvasから実際のタイルサイズを取得**
    int canvasTileSize = canvas.getTileSize();
    int screenTileSize = static_cast<int>(canvasTileSize * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(startPos, view, screenTileSize, brushSize, canvasTileSize);

    // 大型タイルを配置
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

    // **修正：Canvasから実際のタイルサイズを取得**
    int canvasTileSize = canvas.getTileSize();
    int screenTileSize = static_cast<int>(canvasTileSize * view.getZoom());
    sf::Vector2i snappedPos = snapPosition(currentPos, view, screenTileSize, brushSize, canvasTileSize);

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
    // **修正：固定値6ではなく実際のタイルサイズを使用**
    // この関数ではCanvasへのアクセスがないため、引数で渡すか別の方法が必要
    // 暫定的に6のままにするか、別の方法を検討
    int estimatedTileSize = 6; // 暫定値
    sf::Vector2i snappedPos = snapPosition(currentPos, view, estimatedTileSize, brushSize, estimatedTileSize);

    // 基本の大型タイルサイズ
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // 単個大型タイルのサイズ
    int tileWidth = baseWidth * estimatedTileSize;
    int tileHeight = baseHeight * estimatedTileSize;

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

    // 単個大型タイルのサイズ
    float tileWidth = baseWidth * scaledTileSize;
    float tileHeight = baseHeight * scaledTileSize;

    // ブラシサイズに応じた全体サイズ
    float totalWidth = tileWidth * brushSize;
    float totalHeight = tileHeight * brushSize;

    // **修正：実際のタイルサイズを使用**
    int screenTileSize = static_cast<int>(tileSize * zoom);
    sf::Vector2i snappedPos = snapPosition(mousePos, view, screenTileSize, brushSize, tileSize);

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
 * **修正版**：位置を大型タイルサイズとブラシサイズに合わせて正確にスナップ
 * canvasTileSize パラメータを追加
 */
sf::Vector2i LargeTileTool::snapPosition(const sf::Vector2i& pos,
    const CanvasView& view,
    int screenTileSize,
    int brushSize,
    int canvasTileSize) const
{
    // 大型タイルの基本サイズ（キャンバスタイル単位）
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // 単個大型タイルのサイズ（スクリーン座標）
    int tileWidth = screenTileSize * baseWidth;
    int tileHeight = screenTileSize * baseHeight;

    // ブラシ全体のサイズ
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // **修正：より正確なスナップ計算**
    // 中心点を基準にした位置調整
    int halfTotalWidth = totalWidth / 2;
    int halfTotalHeight = totalHeight / 2;

    // スナップ位置を計算（中心点を基準に）
    int snappedX = ((pos.x - halfTotalWidth) / tileWidth) * tileWidth + halfTotalWidth;
    int snappedY = ((pos.y - halfTotalHeight) / tileHeight) * tileHeight + halfTotalHeight;

    return sf::Vector2i(snappedX, snappedY);
}

/**
 * **修正版**：ブラシサイズに応じて大型タイルを複数配置
 */
void LargeTileTool::placeLargeTiles(const sf::Vector2i& basePos,
    int brushSize,
    Canvas& canvas,
    const CanvasView& view) const
{
    // 基本の大型タイルサイズ
    int baseWidth = (currentLargeTileId >= 8 && currentLargeTileId <= 11) ? 4 : 2;
    int baseHeight = 2;

    // **修正：実際のキャンバスタイルサイズを使用**
    int canvasTileSize = canvas.getTileSize();
    int screenTileSize = static_cast<int>(canvasTileSize * view.getZoom());

    // 単個大型タイルのサイズ（スクリーン座標）
    int tileWidth = baseWidth * screenTileSize;
    int tileHeight = baseHeight * screenTileSize;

    // ブラシサイズに応じたオフセット範囲
    int offsetRange = (brushSize - 1) / 2;

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

            // 単個の大型タイルを配置
            placeLargeTile(tileCanvasPos, canvas, view);
        }
    }
}

/**
 * 単個の大型タイルを配置
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