//===== DrawingManager.cpp =====
#include "DrawingManager.hpp"
#include "Canvas.hpp"
#include "CanvasView.hpp"

/**
 * 描画開始
 * 選択されたツールの描画開始処理を呼び出す
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

    // 現在のツールに描画開始を通知
    DrawingTool* tool = toolManager.getCurrentTool();
    if (tool) {
        tool->onDrawStart(startPos, canvas, view, patternIndex, brushSize);
    }
}

/**
 * 描画終了
 * 選択されたツールの描画終了処理を呼び出す
 */
void DrawingManager::stopDrawing(const sf::Vector2i& endPos,
    Canvas& canvas,
    const CanvasView& view,
    int patternIndex,
    int brushSize) {
    if (!isDrawing) return;

    DrawingTool* tool = toolManager.getCurrentTool();
    if (tool) {
        // 単発クリック（移動なし）の場合の特別処理
        if (!hasMoved) {
            // 連続描画をサポートしないツール（直線等）では何もしない
            // 連続描画ツール（ブラシ等）では開始時に既に描画済み
            if (!tool->supportsContinuousDrawing()) {
                // 直線等では開始位置と終了位置が同じ場合は点を描画
                tool->onDrawEnd(endPos, mouseDownPos, canvas, view, patternIndex, brushSize);
            }
        }
        else {
            // 移動ありの場合は終了処理を実行
            tool->onDrawEnd(endPos, mouseDownPos, canvas, view, patternIndex, brushSize);
        }
    }

    isDrawing = false;
}

/**
 * マウス移動の更新
 * 連続描画ツールの場合は描画継続処理を呼び出す
 */
void DrawingManager::updateMovement(const sf::Vector2i& currentPos,
    Canvas& canvas,
    const CanvasView& view,
    int patternIndex,
    int brushSize) {
    // 移動判定を更新
    updateMovementFlag(currentPos);

    // 描画中かつツールが連続描画をサポートする場合
    if (isDrawing && hasMoved) {
        DrawingTool* tool = toolManager.getCurrentTool();
        if (tool && tool->supportsContinuousDrawing()) {
            // 連続描画処理を実行
            tool->onDrawContinue(currentPos, lastMousePos, canvas, view, patternIndex, brushSize);
            lastMousePos = currentPos;
        }
    }
}

/**
 * カーソル描画
 * 現在のツールに応じたカーソルを表示
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
 * プレビュー描画
 * 直線ツール等でドラッグ中のプレビューを表示
 */
void DrawingManager::drawPreview(sf::RenderWindow& window,
    const sf::Vector2i& currentMousePos,
    const CanvasView& view,
    int brushSize) const {
    // 描画中かつ移動ありの場合のみプレビュー表示
    if (isDrawing && hasMoved) {
        DrawingTool* tool = toolManager.getCurrentTool();
        if (tool) {
            tool->drawPreview(window, mouseDownPos, currentMousePos, view, brushSize);
        }
    }
}

/**
 * 描画統計情報を取得
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
 * 移動判定の更新
 * マウスがクリック判定しきい値を超えて移動したかチェック
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