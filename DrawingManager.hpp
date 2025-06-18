#pragma once
//===== DrawingManager.hpp（ツール対応版） =====
#pragma once
#include <SFML/Graphics.hpp>
#include "DrawingTools.hpp"
#include <memory>

// 前方宣言
class Canvas;
class CanvasView;

/**
 * 描画処理を統合管理するクラス
 * 新しいツールシステムに対応し、様々な描画ツールを統一的に扱う
 */
class DrawingManager {
private:
    // 描画状態管理
    bool isDrawing = false;                    // 現在描画中かどうか
    sf::Vector2i lastMousePos;                // 前回のマウス位置（スクリーン座標）
    sf::Vector2i mouseDownPos;                // マウスを押した位置（スクリーン座標）
    bool hasMoved = false;                     // マウスが移動したかどうか
    static const int CLICK_THRESHOLD = 3;     // クリック判定のしきい値（ピクセル）

    // ツール管理
    ToolManager toolManager;                   // ツールマネージャー

public:
    /**
     * コンストラクタ
     */
    DrawingManager() = default;

    // ===== 描画状態管理 =====

    /**
     * 描画開始
     * @param startPos 開始位置（スクリーン座標）
     * @param canvas 描画対象キャンバス
     * @param view CanvasViewインスタンス
     * @param patternIndex 選択されたパターンインデックス
     * @param brushSize ブラシサイズ
     */
    void startDrawing(const sf::Vector2i& startPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize);

    /**
     * 描画終了
     * @param endPos 終了位置（スクリーン座標）
     * @param canvas 描画対象キャンバス
     * @param view CanvasViewインスタンス
     * @param patternIndex 選択されたパターンインデックス
     * @param brushSize ブラシサイズ
     */
    void stopDrawing(const sf::Vector2i& endPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize);

    /**
     * マウス移動の更新
     * @param currentPos 現在のマウス位置（スクリーン座標）
     * @param canvas 描画対象キャンバス
     * @param view CanvasViewインスタンス
     * @param patternIndex 選択されたパターンインデックス
     * @param brushSize ブラシサイズ
     */
    void updateMovement(const sf::Vector2i& currentPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize);

    /**
     * 最後のマウス位置を更新
     * @param pos 新しい位置（スクリーン座標）
     */
    void updateLastMousePos(const sf::Vector2i& pos) {
        lastMousePos = pos;
    }

    // ===== 状態取得 =====

    /**
     * 現在描画中かどうか
     */
    bool getIsDrawing() const { return isDrawing; }

    /**
     * マウスが移動したかどうか
     */
    bool getHasMoved() const { return hasMoved; }

    /**
     * マウスを押した位置を取得
     */
    sf::Vector2i getMouseDownPos() const { return mouseDownPos; }

    /**
     * 前回のマウス位置を取得
     */
    sf::Vector2i getLastMousePos() const { return lastMousePos; }

    // ===== ツール管理 =====

    /**
     * ツールを切り替え
     * @param toolType 切り替え先のツールタイプ
     */
    void setTool(ToolManager::ToolType toolType) {
        toolManager.setTool(toolType);
    }

    /**
     * 現在のツールを取得
     */
    DrawingTool* getCurrentTool() const {
        return toolManager.getCurrentTool();
    }

    /**
     * 現在のツールタイプを取得
     */
    ToolManager::ToolType getCurrentToolType() const {
        return toolManager.getCurrentToolType();
    }

    /**
     * 現在のツール名を取得
     */
    std::string getCurrentToolName() const {
        return toolManager.getCurrentToolName();
    }

    // ===== 描画・表示関連 =====

    /**
     * カーソル描画
     * 現在のツールに応じたカーソルを表示
     * @param window 描画ウィンドウ
     * @param mousePos マウス位置（スクリーン座標）
     * @param view CanvasViewインスタンス
     * @param brushSize ブラシサイズ
     * @param tileSize タイルサイズ
     */
    void drawCursor(sf::RenderWindow& window,
        const sf::Vector2i& mousePos,
        const CanvasView& view,
        int brushSize,
        int tileSize) const;

    /**
     * プレビュー描画
     * 直線ツール等でドラッグ中のプレビューを表示
     * @param window 描画ウィンドウ
     * @param currentMousePos 現在のマウス位置（スクリーン座標）
     * @param view CanvasViewインスタンス
     * @param brushSize ブラシサイズ
     */
    void drawPreview(sf::RenderWindow& window,
        const sf::Vector2i& currentMousePos,
        const CanvasView& view,
        int brushSize) const;

    /**
     * 描画統計情報を取得
     */
    struct DrawingStats {
        bool isDrawing;
        std::string currentTool;
        sf::Vector2i mousePos;
        sf::Vector2i startPos;
        bool hasMoved;
    };

    DrawingStats getDrawingStats() const;

private:
    /**
     * 移動判定の更新
     * マウスがクリック判定しきい値を超えて移動したかチェック
     * @param currentPos 現在のマウス位置
     */
    void updateMovementFlag(const sf::Vector2i& currentPos);
};

