//===== CanvasView.hpp ズーム修正版 =====
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

    // ===== ズーム操作（修正版） =====

    /**
     * マウス位置を中心にズームイン
     */
    void zoomIn(const sf::Vector2i& mousePos) {
        zoomAt(mousePos, ZOOM_STEP);
    }

    /**
     * マウス位置を中心にズームアウト
     */
    void zoomOut(const sf::Vector2i& mousePos) {
        zoomAt(mousePos, 1.0f / ZOOM_STEP);
    }

    /**
     * ズームレベルを直接設定
     */
    void setZoom(float zoom) {
        zoomLevel = std::clamp(zoom, MIN_ZOOM, MAX_ZOOM);
    }

    float getZoom() const { return zoomLevel; }

    // ===== パン操作 =====

    void pan(const sf::Vector2f& delta) {
        panOffset += delta;
    }

    void setPanOffset(const sf::Vector2f& offset) {
        panOffset = offset;
    }

    sf::Vector2f getPanOffset() const { return panOffset; }

    // ===== 座標変換（修正版） =====

    /**
     * スクリーン座標をキャンバス座標に変換（修正版）
     */
    sf::Vector2i screenToCanvas(const sf::Vector2i& screenPos) const {
        // スクリーン座標をキャンバス座標に変換
        sf::Vector2f screenFloat = static_cast<sf::Vector2f>(screenPos);

        // パンオフセットを除去
        sf::Vector2f withoutPan = screenFloat - panOffset;

        // 基準位置からの相対位置を計算
        sf::Vector2f relative = withoutPan - basePosition;

        // ズームを除去して元のキャンバス座標を取得
        sf::Vector2f canvasFloat = relative / zoomLevel + basePosition;

        return static_cast<sf::Vector2i>(canvasFloat);
    }

    /**
     * キャンバス座標をスクリーン座標に変換（修正版）
     */
    sf::Vector2i canvasToScreen(const sf::Vector2i& canvasPos) const {
        sf::Vector2f canvasFloat = static_cast<sf::Vector2f>(canvasPos);

        // 基準位置からの相対位置を計算
        sf::Vector2f relative = canvasFloat - basePosition;

        // ズームを適用
        sf::Vector2f zoomed = relative * zoomLevel;

        // 基準位置とパンオフセットを適用
        sf::Vector2f screenFloat = zoomed + basePosition + panOffset;

        return static_cast<sf::Vector2i>(screenFloat);
    }

    // ===== 描画用変換行列（修正版） =====

    sf::Transform getTransform() const {
        sf::Transform transform;

        // 1. 基準位置に移動
        transform.translate(basePosition);

        // 2. パンオフセットを適用
        transform.translate(panOffset);

        // 3. ズームを適用
        transform.scale(zoomLevel, zoomLevel);

        // 4. 基準位置を原点に戻す
        transform.translate(-basePosition);

        return transform;
    }

    // ===== 表示判定 =====

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

    // ===== 設定更新 =====

    void updateWindowSize(const sf::Vector2u& newSize) {
        windowSize = newSize;
    }

    void setBasePosition(const sf::Vector2f& newBasePos) {
        basePosition = newBasePos;
    }

    // ===== ビューのリセット =====

    /**
     * ビュー状態をリセット（ズーム1.0、パンオフセット0）
     */
    void reset() {
        zoomLevel = 1.0f;
        panOffset = sf::Vector2f(0.0f, 0.0f);
    }

    /**
     * マウスホイールによるズーム制御（修正版）
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
     * 現在のビュー状態を文字列として取得
     */
    std::string getStatusString() const {
        std::ostringstream oss;
        oss << "Zoom: " << std::fixed << std::setprecision(1) << (zoomLevel * 100.0f) << "%";
        oss << " | Pan: (" << static_cast<int>(panOffset.x) << ", " << static_cast<int>(panOffset.y) << ")";
        return oss.str();
    }

private:
    /**
     * 指定座標を中心にズーム実行（修正版）
     */
    void zoomAt(const sf::Vector2i& centerPos, float factor) {
        float oldZoom = zoomLevel;
        float newZoom = std::clamp(zoomLevel * factor, MIN_ZOOM, MAX_ZOOM);

        if (std::abs(newZoom - oldZoom) < 0.001f) return;

        // ズーム前のマウス位置でのキャンバス座標を取得
        sf::Vector2i canvasPosBeforeZoom = screenToCanvas(centerPos);

        // ズームレベル更新
        zoomLevel = newZoom;

        // ズーム後の同じキャンバス座標のスクリーン位置を計算
        sf::Vector2i screenPosAfterZoom = canvasToScreen(canvasPosBeforeZoom);

        // マウス位置が変わらないようにパンオフセットを調整
        sf::Vector2f correction = static_cast<sf::Vector2f>(centerPos - screenPosAfterZoom);
        panOffset += correction;
    }
};