//===== LargeTileSystem.cpp 完全版（回転機能対応） =====
#include "LargeTileSystem.hpp"
#include "Canvas.hpp"
#include "CanvasView.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ===== グローバル変数の定義（リンクエラー解決） =====
LargeTileManager largeTileManager;

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

    // 実際のキャンバスタイルサイズを取得
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

    // 実際のキャンバスタイルサイズを取得
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
 * プレビュー描画（回転対応）
 */
void LargeTileTool::drawPreview(sf::RenderWindow& window,
    const sf::Vector2i& startPos,
    const sf::Vector2i& currentPos,
    const CanvasView& view,
    int brushSize) const
{
    // 現在の大型タイルの回転状態を取得
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();
    RotationAngle rotation = currentTile.getCurrentRotation();

    int estimatedTileSize = 6; // 暫定値
    sf::Vector2i snappedPos = snapPosition(currentPos, view, estimatedTileSize, brushSize, estimatedTileSize);

    // 回転を考慮したサイズ計算
    int baseWidth, baseHeight;
    if (currentLargeTileId >= 8 && currentLargeTileId <= 11) {
        // 4x2タイル
        if (rotation == RotationAngle::ROTATE_90 || rotation == RotationAngle::ROTATE_270) {
            baseWidth = 2; baseHeight = 4; // 90度/270度回転で2x4になる
        }
        else {
            baseWidth = 4; baseHeight = 2; // 0度/180度で4x2のまま
        }
    }
    else {
        // 2x2タイル（回転しても2x2のまま）
        baseWidth = 2; baseHeight = 2;
    }

    // 単個大型タイルのサイズ
    int tileWidth = baseWidth * estimatedTileSize;
    int tileHeight = baseHeight * estimatedTileSize;

    // ブラシサイズに応じた全体サイズ
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // プレビュー矩形描画（半透明）
    sf::RectangleShape preview(sf::Vector2f(totalWidth, totalHeight));
    preview.setPosition(static_cast<sf::Vector2f>(snappedPos) - sf::Vector2f(totalWidth / 2, totalHeight / 2));
    preview.setFillColor(sf::Color(255, 255, 255, 30)); // より薄い半透明

    // 回転状態に応じた枠色
    sf::Color frameColor = sf::Color(255, 255, 0, 100); // デフォルト：黄色
    switch (rotation) {
    case RotationAngle::ROTATE_90:  frameColor = sf::Color(0, 255, 255, 100); break;  // シアン
    case RotationAngle::ROTATE_180: frameColor = sf::Color(255, 0, 255, 100); break;  // マゼンタ
    case RotationAngle::ROTATE_270: frameColor = sf::Color(255, 165, 0, 100); break;  // オレンジ
    default: break;
    }

    preview.setOutlineThickness(2.0f);
    preview.setOutlineColor(frameColor);
    window.draw(preview);

    // 大型タイル番号表示
    sf::CircleShape marker(8.0f);
    marker.setOrigin(8.0f, 8.0f);
    marker.setPosition(static_cast<sf::Vector2f>(snappedPos));
    marker.setFillColor(sf::Color(255, 0, 0, 150));
    window.draw(marker);
}

/**
 * カーソル描画（回転対応）
 */
void LargeTileTool::drawCursor(sf::RenderWindow& window,
    const sf::Vector2i& mousePos,
    const CanvasView& view,
    int brushSize,
    int tileSize) const
{
    float zoom = view.getZoom();
    float scaledTileSize = tileSize * zoom;

    // 現在の大型タイルの回転状態を取得
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();
    RotationAngle rotation = currentTile.getCurrentRotation();

    // 基本の大型タイルサイズ（回転を考慮）
    int baseWidth, baseHeight;
    if (currentLargeTileId >= 8 && currentLargeTileId <= 11) {
        // 4x2タイル
        if (rotation == RotationAngle::ROTATE_90 || rotation == RotationAngle::ROTATE_270) {
            baseWidth = 2; baseHeight = 4; // 90度/270度回転で2x4になる
        }
        else {
            baseWidth = 4; baseHeight = 2; // 0度/180度で4x2のまま
        }
    }
    else {
        // 2x2タイル（回転しても2x2のまま）
        baseWidth = 2; baseHeight = 2;
    }

    // 単個大型タイルのサイズ
    float tileWidth = baseWidth * scaledTileSize;
    float tileHeight = baseHeight * scaledTileSize;

    // ブラシサイズに応じた全体サイズ
    float totalWidth = tileWidth * brushSize;
    float totalHeight = tileHeight * brushSize;

    // スナップ位置を計算
    int screenTileSize = static_cast<int>(tileSize * zoom);
    sf::Vector2i snappedPos = snapPosition(mousePos, view, screenTileSize, brushSize, tileSize);

    // 大型タイル枠描画
    sf::RectangleShape largeTileFrame(sf::Vector2f(totalWidth, totalHeight));
    largeTileFrame.setPosition(static_cast<sf::Vector2f>(snappedPos) - sf::Vector2f(totalWidth / 2, totalHeight / 2));
    largeTileFrame.setFillColor(sf::Color::Transparent);
    largeTileFrame.setOutlineThickness(2.0f);

    // 回転状態に応じて枠の色を変更
    sf::Color frameColor = sf::Color(255, 255, 0, 200); // デフォルト：黄色
    switch (rotation) {
    case RotationAngle::ROTATE_90:  frameColor = sf::Color(0, 255, 255, 200); break;  // シアン
    case RotationAngle::ROTATE_180: frameColor = sf::Color(255, 0, 255, 200); break;  // マゼンタ
    case RotationAngle::ROTATE_270: frameColor = sf::Color(255, 165, 0, 200); break;  // オレンジ
    default: break;
    }
    largeTileFrame.setOutlineColor(frameColor);
    window.draw(largeTileFrame);

    // 内部グリッド線描画
    sf::Color boundaryColor(255, 255, 255, 150);
    float lineThickness = 1.0f;

    // 縦線（ブラシサイズ単位）
    for (int i = 1; i < brushSize; i++) {
        float xPos = largeTileFrame.getPosition().x + i * tileWidth;
        sf::RectangleShape vLine(sf::Vector2f(lineThickness, totalHeight));
        vLine.setPosition(xPos, largeTileFrame.getPosition().y);
        vLine.setFillColor(boundaryColor);
        window.draw(vLine);
    }

    // 横線（ブラシサイズ単位）
    for (int i = 1; i < brushSize; i++) {
        float yPos = largeTileFrame.getPosition().y + i * tileHeight;
        sf::RectangleShape hLine(sf::Vector2f(totalWidth, lineThickness));
        hLine.setPosition(largeTileFrame.getPosition().x, yPos);
        hLine.setFillColor(boundaryColor);
        window.draw(hLine);
    }

    // 大型タイル番号＋回転角度表示
    sf::CircleShape idMarker(8.0f);
    idMarker.setOrigin(8.0f, 8.0f);
    idMarker.setPosition(static_cast<sf::Vector2f>(snappedPos));
    idMarker.setFillColor(frameColor);
    idMarker.setOutlineThickness(1.0f);
    idMarker.setOutlineColor(sf::Color::White);
    window.draw(idMarker);

    // 回転角度インジケーター（中央の小さな矢印）
    if (rotation != RotationAngle::ROTATE_0) {
        drawRotationIndicator(window, snappedPos, rotation);
    }
}

/**
 * 位置を大型タイルサイズとブラシサイズに合わせて正確にスナップ
 */
sf::Vector2i LargeTileTool::snapPosition(const sf::Vector2i& pos,
    const CanvasView& view,
    int screenTileSize,
    int brushSize,
    int canvasTileSize) const
{
    // 現在の大型タイルの回転状態を取得
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();
    RotationAngle rotation = currentTile.getCurrentRotation();

    // 大型タイルの基本サイズ（回転考慮）
    int baseWidth, baseHeight;
    if (currentLargeTileId >= 8 && currentLargeTileId <= 11) {
        // 4x2タイル
        if (rotation == RotationAngle::ROTATE_90 || rotation == RotationAngle::ROTATE_270) {
            baseWidth = 2; baseHeight = 4;
        }
        else {
            baseWidth = 4; baseHeight = 2;
        }
    }
    else {
        // 2x2タイル
        baseWidth = 2; baseHeight = 2;
    }

    // 単個大型タイルのサイズ（スクリーン座標）
    int tileWidth = screenTileSize * baseWidth;
    int tileHeight = screenTileSize * baseHeight;

    // ブラシ全体のサイズ
    int totalWidth = tileWidth * brushSize;
    int totalHeight = tileHeight * brushSize;

    // より正確なスナップ計算
    int halfTotalWidth = totalWidth / 2;
    int halfTotalHeight = totalHeight / 2;

    // スナップ位置を計算（中心点を基準に）
    int snappedX = ((pos.x - halfTotalWidth) / tileWidth) * tileWidth + halfTotalWidth;
    int snappedY = ((pos.y - halfTotalHeight) / tileHeight) * tileHeight + halfTotalHeight;

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
    // 現在の大型タイルの回転状態を取得
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();
    RotationAngle rotation = currentTile.getCurrentRotation();

    // 基本の大型タイルサイズ（回転考慮）
    int baseWidth, baseHeight;
    if (currentLargeTileId >= 8 && currentLargeTileId <= 11) {
        // 4x2タイル
        if (rotation == RotationAngle::ROTATE_90 || rotation == RotationAngle::ROTATE_270) {
            baseWidth = 2; baseHeight = 4;
        }
        else {
            baseWidth = 4; baseHeight = 2;
        }
    }
    else {
        // 2x2タイル
        baseWidth = 2; baseHeight = 2;
    }

    // 実際のキャンバスタイルサイズを使用
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
    // グローバルのLargeTileManagerから現在の大型タイルを取得
    const LargeTile& currentTile = largeTileManager.getCurrentLargeTile();

    auto arrangement = currentTile.getArrangement(); // 回転が適用された配置を取得
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
 * 回転インジケーターを描画
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

    // 矢印線
    sf::VertexArray arrow(sf::Lines, 2);
    arrow[0] = sf::Vertex(arrowStart, sf::Color::White);
    arrow[1] = sf::Vertex(arrowEnd, sf::Color::White);
    window.draw(arrow);

    // 矢印の先端
    sf::CircleShape arrowHead(1.5f);
    arrowHead.setOrigin(1.5f, 1.5f);
    arrowHead.setPosition(arrowEnd);
    arrowHead.setFillColor(sf::Color::White);
    window.draw(arrowHead);
}