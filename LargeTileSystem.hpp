//===== LargeTileSystem.hpp 大型タイルシステム =====
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include "DrawingTools.hpp"

/**
 * 大型タイル情報を管理するクラス
 * 2x2または4x2のタイル配置をサポート
 */
class LargeTile {
public:
    // タイル配置タイプ
    enum class ArrangementType {
        TILE_2x2,  // 2x2配置 (4タイル)
        TILE_4x2   // 4x2配置 (8タイル)
    };

    // タイル配置情報
    struct TileArrangement {
        ArrangementType type;
        std::vector<int> indices;  // タイルインデックス配列
    };

private:
    int largeTileId;              // 大型タイル番号（0-11）
    TileArrangement arrangement;  // タイル配置情報

    /**
    * **修正版**：位置を大型タイルサイズとブラシサイズにスナップ
    * @param pos 元の位置
    * @param view CanvasViewインスタンス
    * @param screenTileSize スクリーン上のタイルサイズ
    * @param brushSize ブラシサイズ
    * @param canvasTileSize キャンバスの実際のタイルサイズ
    */
   


public:
    LargeTile(int id) : largeTileId(id) {
        calculateArrangement();
    }

    /**
     * 大型タイル番号からタイル配置を計算
     */
    void calculateArrangement() {
        arrangement.indices.clear();

        if (largeTileId >= 0 && largeTileId <= 7) {
            // 2x2パターン (4タイル)
            arrangement.type = ArrangementType::TILE_2x2;
            int baseRow = largeTileId / 2;
            int baseCol = largeTileId % 2;
            int baseIndex = baseRow * 8 + baseCol * 2;

            arrangement.indices = {
                baseIndex,      // topLeft
                baseIndex + 1,  // topRight
                baseIndex + 4,  // bottomLeft
                baseIndex + 5   // bottomRight
            };
        }
        else if (largeTileId >= 8 && largeTileId <= 11) {
            // 4x2パターン (8タイル)
            arrangement.type = ArrangementType::TILE_4x2;
            int patternIndex = largeTileId - 8;
            int baseIndex = 32 + patternIndex * 8;

            arrangement.indices = {
                baseIndex,      // row0 col0
                baseIndex + 1,  // row0 col1
                baseIndex + 2,  // row0 col2
                baseIndex + 3,  // row0 col3
                baseIndex + 4,  // row1 col0
                baseIndex + 5,  // row1 col1
                baseIndex + 6,  // row1 col2
                baseIndex + 7   // row1 col3
            };
        }
    }

    // ゲッター
    int getId() const { return largeTileId; }
    const TileArrangement& getArrangement() const { return arrangement; }

    /**
     * 描画位置を計算
     */
    std::vector<sf::Vector2i> getDrawPositions(
        const sf::Vector2i& basePos,
        int tileSize
    ) const {
        std::vector<sf::Vector2i> positions;

        if (arrangement.type == ArrangementType::TILE_2x2) {
            // 2x2配置
            positions = {
                basePos,                                      // 左上
                sf::Vector2i(basePos.x + tileSize, basePos.y),     // 右上
                sf::Vector2i(basePos.x, basePos.y + tileSize),     // 左下
                sf::Vector2i(basePos.x + tileSize, basePos.y + tileSize) // 右下
            };
        }
        else {
            // 4x2配置
            for (int row = 0; row < 2; row++) {
                for (int col = 0; col < 4; col++) {
                    positions.push_back(sf::Vector2i(
                        basePos.x + col * tileSize,
                        basePos.y + row * tileSize
                    ));
                }
            }
        }
        return positions;
    }

    /**
     * デバッグ情報取得
     */
    std::string getDebugInfo() const {
        std::string info = "LargeTile[" + std::to_string(largeTileId) + "]: ";
        for (int index : arrangement.indices) {
            info += std::to_string(index) + ",";
        }
        info.pop_back(); // 最後のカンマを削除
        return info;
    }
};

// ... (LargeTileToolとLargeTileManagerは変更なし) ...
/**
 * 大型タイルツール
 * クリック位置に2x2の大型タイルを配置
 */
class LargeTileTool : public DrawingTool {
private:
    int currentLargeTileId = 0;  // 現在選択中の大型タイル番号
    bool isDrawing = false;              // 描画中フラグ
    sf::Vector2i startDrawPos;          // 描画開始位置
    sf::Vector2i lastPlacedPos;         // 最後に配置した位置

public:
    void onDrawStart(const sf::Vector2i& startPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize) override;

    void onDrawContinue(const sf::Vector2i& currentPos,
        const sf::Vector2i& lastPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize) override;

    void onDrawEnd(const sf::Vector2i& endPos,
        const sf::Vector2i& startPos,
        Canvas& canvas,
        const CanvasView& view,
        int patternIndex,
        int brushSize) override;

    void drawPreview(sf::RenderWindow& window,
        const sf::Vector2i& startPos,
        const sf::Vector2i& currentPos,
        const CanvasView& view,
        int brushSize) const override;

    void drawCursor(sf::RenderWindow& window,
        const sf::Vector2i& mousePos,
        const CanvasView& view,
        int brushSize,
        int tileSize) const override;

    std::string getToolName() const override { return "Large Tile"; }
    bool supportsContinuousDrawing() const override { return true; }

    /**
     * 大型タイル番号を設定
     * @param largeTileId 0-11の大型タイル番号
     */
    void setLargeTileId(int largeTileId) {
        if (largeTileId >= 0 && largeTileId <= 11) {
            currentLargeTileId = largeTileId;
        }
    }

    /**
     * 現在の大型タイル番号を取得
     */
    int getLargeTileId() const {
        return currentLargeTileId;
    }

private:
    /**
     * 大型タイルをキャンバスに配置
     * @param basePos 配置基準位置（スクリーン座標）
     * @param canvas 描画対象キャンバス
     * @param view CanvasViewインスタンス
     */
     /**
       * 位置を大型タイルサイズとブラシサイズにスナップ
       */
    sf::Vector2i snapPosition(const sf::Vector2i& pos,
        const CanvasView& view,
        int screenTileSize,
        int brushSize,
        int canvasTileSize) const;

    /**
       * 単個の大型タイルを配置
       */
    void placeLargeTile(const sf::Vector2i& canvasPos,
        Canvas& canvas,
        const CanvasView& view) const;

    /**
     * ブラシサイズに応じて複数の大型タイルを配置
     */
    void placeLargeTiles(const sf::Vector2i& basePos,
        int brushSize,
        Canvas& canvas,
        const CanvasView& view) const;
};

/**
 * 大型タイル管理クラス
 * 大型タイルの選択と情報管理
 */
class LargeTileManager {
private:
    std::vector<LargeTile> largeTiles;
    int currentSelection = 0;

public:
    LargeTileManager() {
        // 12個の大型タイルを初期化
        for (int i = 0; i < 12; ++i) {
            largeTiles.emplace_back(i);
        }
    }

    /**
     * 大型タイルを選択
     * @param id 0-11の大型タイル番号
     */
    void selectLargeTile(int id) {
        if (id >= 0 && id < 12) {
            currentSelection = id;
        }
    }

    /**
     * 現在選択中の大型タイルを取得
     */
    const LargeTile& getCurrentLargeTile() const {
        return largeTiles[currentSelection];
    }

    /**
     * 指定番号の大型タイルを取得
     */
    const LargeTile& getLargeTile(int id) const {
        if (id >= 0 && id < 12) {
            return largeTiles[id];
        }
        return largeTiles[0];  // フォールバック
    }

    /**
     * 現在の選択番号を取得
     */
    int getCurrentSelection() const {
        return currentSelection;
    }

    /**
     * 全大型タイルの情報をデバッグ出力
     */
    void printDebugInfo() const {
        for (const auto& tile : largeTiles) {
            std::cout << tile.getDebugInfo() << std::endl;
        }
    }
};