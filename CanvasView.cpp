//===== Canvas.cpp CanvasView対応実装（既存コード拡張） =====
// 既存のCanvas.cppに以下のメソッドを追加

#include "Canvas.hpp"
#include "CanvasView.hpp"
#include <iostream>

// 既存のメソッドはそのまま保持...

/**
 * CanvasViewに対応した描画メソッド
 * 既存のdraw()メソッドをベースにズーム・パン対応
 */
void Canvas::drawWithView(sf::RenderWindow& window,
    const CanvasView& view,
    const std::vector<std::vector<int>>& patterns,
    const std::vector<std::array<sf::Color, 3>>& colorPalettes,
    bool showGrid,
    float spacing,
    float shrink) {

    // RenderTextureの初期化
    if (!isInitialized) {
        initializeRenderTexture();
    }

    // 設定変更の検出
    bool settingsChanged = (showGrid != lastShowGrid ||
        spacing != lastSpacing ||
        shrink != lastShrink);

    // ダーティフラグまたは設定変更時の再描画
    if (isDirty || settingsChanged) {
        renderToTexture(patterns, colorPalettes, showGrid, spacing, shrink);
        isDirty = false;
        lastShowGrid = showGrid;
        lastSpacing = spacing;
        lastShrink = shrink;
    }

    // CanvasViewの変換を適用してテクスチャを描画
    if (isInitialized) {
        sf::Sprite sprite(renderTexture.getTexture());

        // CanvasViewの変換行列を適用
        sf::RenderStates states;
        states.transform = view.getTransform();

        // スプライトの位置を設定
        sprite.setPosition(position);

        window.draw(sprite, states);

        // 境界線を描画
        sf::RectangleShape border(sf::Vector2f(width * tileSize, height * tileSize));
        border.setPosition(position);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1.0f / view.getZoom()); // ズームに応じた線の太さ
        border.setOutlineColor(sf::Color(100, 100, 100));
        window.draw(border, states);
    }
}

/**
 * CanvasView座標系でのクリック判定
 */
bool Canvas::containsInView(const CanvasView& view, const sf::Vector2i& screenPos) const {
    // スクリーン座標をキャンバス座標に変換
    sf::Vector2i canvasPos = view.screenToCanvas(screenPos);

    // キャンバス領域内かチェック
    sf::FloatRect canvasRect(position.x, position.y, width * tileSize, height * tileSize);
    return canvasRect.contains(static_cast<sf::Vector2f>(canvasPos));
}

/**
 * CanvasView座標系でのタイル操作
 */
void Canvas::handleClickInView(const CanvasView& view, const sf::Vector2i& screenPos, int patternIndex) {
    if (patternIndex < 0) return;

    // スクリーン座標をキャンバス座標に変換
    sf::Vector2i canvasPos = view.screenToCanvas(screenPos);

    // キャンバス領域内かチェック
    if (!containsInView(view, screenPos)) return;

    // タイルインデックスを計算
    sf::Vector2i tileIndex = screenToTileIndex(view, screenPos);

    if (tileIndex.x >= 0 && tileIndex.x < width &&
        tileIndex.y >= 0 && tileIndex.y < height) {

        // 変更がある場合のみダーティフラグを設定
        if (tiles[tileIndex.y][tileIndex.x] != patternIndex) {
            tiles[tileIndex.y][tileIndex.x] = patternIndex;
            isDirty = true;
        }
    }
}

/**
 * CanvasView座標系でのタイル消去
 */
void Canvas::eraseTileInView(const CanvasView& view, const sf::Vector2i& screenPos) {
    // スクリーン座標をキャンバス座標に変換
    if (!containsInView(view, screenPos)) return;

    // タイルインデックスを計算
    sf::Vector2i tileIndex = screenToTileIndex(view, screenPos);

    if (tileIndex.x >= 0 && tileIndex.x < width &&
        tileIndex.y >= 0 && tileIndex.y < height) {

        // 既に空の場合は何もしない
        if (tiles[tileIndex.y][tileIndex.x] != -1) {
            tiles[tileIndex.y][tileIndex.x] = -1;
            isDirty = true;
        }
    }
}

/**
 * パフォーマンス情報取得
 */
Canvas::PerformanceInfo Canvas::getPerformanceInfo(const CanvasView& view) const {
    PerformanceInfo info;
    info.totalTiles = width * height;
    info.zoomLevel = view.getZoom();

    // 表示範囲内のタイル数を計算
    sf::FloatRect visibleArea = view.getVisibleCanvasArea();

    // キャンバス座標系での表示範囲を計算
    int startX = std::max(0, static_cast<int>((visibleArea.left - position.x) / tileSize));
    int startY = std::max(0, static_cast<int>((visibleArea.top - position.y) / tileSize));
    int endX = std::min(width, static_cast<int>((visibleArea.left + visibleArea.width - position.x) / tileSize) + 1);
    int endY = std::min(height, static_cast<int>((visibleArea.top + visibleArea.height - position.y) / tileSize) + 1);

    info.visibleTiles = (endX - startX) * (endY - startY);

    // 実際に描画されるタイル数をカウント
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
 * CanvasView用の座標変換ヘルパー
 */
sf::Vector2i Canvas::screenToTileIndex(const CanvasView& view, const sf::Vector2i& screenPos) const {
    // スクリーン座標をキャンバス座標に変換
    sf::Vector2i canvasPos = view.screenToCanvas(screenPos);

    // キャンバス座標からタイルインデックスを計算
    sf::Vector2f localPos = static_cast<sf::Vector2f>(canvasPos) - position;
    int tileX = static_cast<int>(localPos.x) / tileSize;
    int tileY = static_cast<int>(localPos.y) / tileSize;

    return sf::Vector2i(tileX, tileY);
}