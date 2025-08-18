//===== LargeTileSystem.hpp 完全版（回転機能対応） =====
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <cmath>
#include "DrawingTools.hpp"

//#include "LargeTileManager.hpp"

/**
 * 回転角度の列挙型
 */
enum class RotationAngle {
    ROTATE_0 = 0,   // 0度（デフォルト）
    ROTATE_90 = 90,  // 90度時計回り
    ROTATE_180 = 180, // 180度
    ROTATE_270 = 270  // 270度時計回り
};

/**
 * 大型タイル回転管理クラス
 */
class LargeTileRotation {
private:
    RotationAngle currentRotation = RotationAngle::ROTATE_0;

public:
    /**
     * 次の回転角度に進む（90度ずつ循環）
     */
    void rotateNext() {
        switch (currentRotation) {
        case RotationAngle::ROTATE_0:   currentRotation = RotationAngle::ROTATE_90;  break;
        case RotationAngle::ROTATE_90:  currentRotation = RotationAngle::ROTATE_180; break;
        case RotationAngle::ROTATE_180: currentRotation = RotationAngle::ROTATE_270; break;
        case RotationAngle::ROTATE_270: currentRotation = RotationAngle::ROTATE_0;   break;
        }
    }

    /**
     * 現在の回転角度を取得
     */
    RotationAngle getCurrentRotation() const {
        return currentRotation;
    }

    /**
     * 回転角度を設定
     */
    void setRotation(RotationAngle angle) {
        currentRotation = angle;
    }

    /**
     * 回転角度を度数で取得
     */
    int getRotationDegrees() const {
        return static_cast<int>(currentRotation);
    }

    /**
     * 回転状態をリセット（0度に戻す）
     */
    void reset() {
        currentRotation = RotationAngle::ROTATE_0;
    }
};

/**
 * 大型タイル情報を管理するクラス（回転機能対応）
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
    TileArrangement baseArrangement;  // 基本配置（0度）
    LargeTileRotation rotation;       // 回転管理

public:
    LargeTile(int id) : largeTileId(id) {
        calculateBaseArrangement();
    }

    /**
     * 基本配置（0度）を計算
     */
    void calculateBaseArrangement() {
        baseArrangement.indices.clear();

        if (largeTileId >= 0 && largeTileId <= 7) {
            // 2x2パターン
            baseArrangement.type = ArrangementType::TILE_2x2;
            int baseRow = largeTileId / 2;
            int baseCol = largeTileId % 2;
            int baseIndex = baseRow * 8 + baseCol * 2;

            baseArrangement.indices = {
                baseIndex,      // [0] 左上
                baseIndex + 1,  // [1] 右上
                baseIndex + 4,  // [2] 左下
                baseIndex + 5   // [3] 右下
            };
        }
        else if (largeTileId >= 8 && largeTileId <= 11) {
            // 4x2パターン
            baseArrangement.type = ArrangementType::TILE_4x2;
            int patternIndex = largeTileId - 8;
            int baseIndex = 32 + patternIndex * 8;

            baseArrangement.indices = {
                baseIndex,      // [0] 
                baseIndex + 1,  // [1]
                baseIndex + 2,  // [2]
                baseIndex + 3,  // [3]
                baseIndex + 4,  // [4]
                baseIndex + 5,  // [5]
                baseIndex + 6,  // [6]
                baseIndex + 7   // [7]
            };
        }
    }

    /**
     * 現在の回転を適用した配置を取得
     */
    TileArrangement getArrangement() const {
        return getRotatedArrangement(rotation.getCurrentRotation());
    }

    /**
     * 指定回転角度での配置を取得
     */
    TileArrangement getRotatedArrangement(RotationAngle angle) const {
        TileArrangement rotated = baseArrangement;

        if (angle == RotationAngle::ROTATE_0) {
            return rotated; // 回転なし
        }

        // インデックスを回転に応じて再配置
        if (baseArrangement.type == ArrangementType::TILE_2x2) {
            rotated.indices = rotate2x2Indices(baseArrangement.indices, angle);
        }
        else {
            rotated.indices = rotate4x2Indices(baseArrangement.indices, angle);
        }

        return rotated;
    }

    /**
     * 回転制御
     */
    void rotateNext() {
        rotation.rotateNext();
    }

    void setRotation(RotationAngle angle) {
        rotation.setRotation(angle);
    }

    RotationAngle getCurrentRotation() const {
        return rotation.getCurrentRotation();
    }

    /**
     * 描画位置を回転に応じて計算
     */
    std::vector<sf::Vector2i> getDrawPositions(const sf::Vector2i& basePos, int tileSize) const {
        std::vector<sf::Vector2i> positions;
        RotationAngle angle = rotation.getCurrentRotation();

        if (baseArrangement.type == ArrangementType::TILE_2x2) {
            positions = getRotated2x2Positions(basePos, tileSize, angle);
        }
        else {
            positions = getRotated4x2Positions(basePos, tileSize, angle);
        }

        return positions;
    }

    // ===== ゲッター =====
    int getId() const { return largeTileId; }

    std::string getDebugInfo() const {
        auto arrangement = getArrangement();
        std::string info = "LargeTile[" + std::to_string(largeTileId) +
            "] Rotation[" + std::to_string(rotation.getRotationDegrees()) + "°]: ";
        for (int index : arrangement.indices) {
            info += std::to_string(index) + ",";
        }
        if (!info.empty()) info.pop_back(); // 最後のカンマを削除
        return info;
    }

private:
    /**
     * 2x2配置のインデックス回転（修正版）
     * 配列順序: [0]=左上, [1]=右上, [2]=左下, [3]=右下
     *
     * 視覚的な回転を実現するため、位置ベースで回転を計算
     */
    std::vector<int> rotate2x2Indices(const std::vector<int>& original, RotationAngle angle) const {
        if (original.size() != 4) return original;

        

        std::vector<int> result;
        
        switch (angle) {
        case RotationAngle::ROTATE_90:
            // 時計回り90度: 左下→左上, 左上→右上, 右上→右下, 右下→左下
            result = { original[2], original[0], original[3], original[1] };
            break;
            
        case RotationAngle::ROTATE_180:
            // 180度: 完全に対角線で入れ替え
            result = { original[3], original[2], original[1], original[0] };
            break;
            
        case RotationAngle::ROTATE_270:
            // 反時計回り90度: 右上→左上, 右下→右上, 左下→右下, 左上→左下
            result = { original[1], original[3], original[0], original[2] };
            break;
            
        default:
            result = original;
            break;
        }



        return result;
    }

    /**
     * 4x2配置のインデックス回転（修正版）
     * 元の配列順序: [0][1][2][3]
     *              [4][5][6][7]
     */
    std::vector<int> rotate4x2Indices(const std::vector<int>& original, RotationAngle angle) const {
        if (original.size() != 8) return original;

        switch (angle) {
        case RotationAngle::ROTATE_90:
            // 4x2 → 2x4 （時計回り90度）
            // 新しい配列: [0][1]
            //            [2][3]
            //            [4][5]
            //            [6][7]
            // 元の位置から新しい位置へのマッピング
            return {
                original[6], original[4],  // 新しい1行目：元の左下列を逆順
                original[7], original[5],  // 新しい2行目：元の右下列を逆順
                original[2], original[0],  // 新しい3行目：元の左上列を逆順  
                original[3], original[1]   // 新しい4行目：元の右上列を逆順
            };

        case RotationAngle::ROTATE_180:
            // 4x2 → 4x2 （180度反転）
            // 全体を反転
            return {
                original[7], original[6], original[5], original[4],
                original[3], original[2], original[1], original[0]
            };

        case RotationAngle::ROTATE_270:
            // 4x2 → 2x4 （反時計回り90度）
            // 90度回転の逆
            return {
                original[1], original[3],  // 新しい1行目：元の右上列
                original[0], original[2],  // 新しい2行目：元の左上列
                original[5], original[7],  // 新しい3行目：元の右下列
                original[4], original[6]   // 新しい4行目：元の左下列
            };

        default:
            return original;
        }
    }

    /**
      * 2x2の描画位置を回転に応じて計算（修正版）
      * より直感的な回転位置計算
      */
    std::vector<sf::Vector2i> getRotated2x2Positions(const sf::Vector2i& basePos, int tileSize, RotationAngle angle) const {
        std::vector<sf::Vector2i> positions;

        // 基本位置（0度）
        std::vector<sf::Vector2i> basePositions = {
            basePos,                                          // [0] 左上
            sf::Vector2i(basePos.x + tileSize, basePos.y),   // [1] 右上
            sf::Vector2i(basePos.x, basePos.y + tileSize),   // [2] 左下
            sf::Vector2i(basePos.x + tileSize, basePos.y + tileSize) // [3] 右下
        };

        positions = basePositions;

        //-------------------------------------------------------------------------------------------------------
        // 並び替えは必要ない？
        //-------------------------------------------------------------------------------------------------------
        /*
        // 回転に応じて位置を並び替え（インデックスの回転と同じロジック）
        switch (angle) {
        case RotationAngle::ROTATE_0:
            positions = basePositions;
            break;
        case RotationAngle::ROTATE_90:
            positions = {
                basePositions[0], // [0]の位置に[2]が来る
                basePositions[1], // [1]の位置に[0]が来る  
                basePositions[2], // [2]の位置に[3]が来る
                basePositions[3]  // [3]の位置に[1]が来る
            };
            break;
        case RotationAngle::ROTATE_180:
            positions = {
                 basePositions[0], // [0]の位置に[2]が来る
                basePositions[1], // [1]の位置に[0]が来る  
                basePositions[2], // [2]の位置に[3]が来る
                basePositions[3]  // [3]の位置に[1]が来る
            };
            break;
        case RotationAngle::ROTATE_270:
            positions = {
                 basePositions[0], // [0]の位置に[2]が来る
                basePositions[1], // [1]の位置に[0]が来る  
                basePositions[2], // [2]の位置に[3]が来る
                basePositions[3]  // [3]の位置に[1]が来る
            };
            break;
        }
        */
        //-------------------------------------------------------------------------------------------------------

        return positions;
    }

    /**
    * 4x2の描画位置を回転に応じて計算（修正版）
    */
    std::vector<sf::Vector2i> getRotated4x2Positions(const sf::Vector2i& basePos, int tileSize, RotationAngle angle) const {
        std::vector<sf::Vector2i> positions;

        switch (angle) {
        case RotationAngle::ROTATE_0:
            // 4x2: 標準配置
            for (int row = 0; row < 2; row++) {
                for (int col = 0; col < 4; col++) {
                    positions.push_back(sf::Vector2i(
                        basePos.x + col * tileSize,
                        basePos.y + row * tileSize
                    ));
                }
            }
            break;

        case RotationAngle::ROTATE_90:
            // 2x4: 90度回転（縦長になる）
            // 4x2の各列が2x4の各行になる
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 2; col++) {
                    positions.push_back(sf::Vector2i(
                        basePos.x + col * tileSize,
                        basePos.y + row * tileSize
                    ));
                }
            }
            break;

        case RotationAngle::ROTATE_180:
            // 4x2: 180度回転（位置は同じ、配置が逆順）
            for (int row = 0; row < 2; row++) {
                for (int col = 0; col < 4; col++) {
                    positions.push_back(sf::Vector2i(
                        basePos.x + col * tileSize,
                        basePos.y + row * tileSize
                    ));
                }
            }
            break;

        case RotationAngle::ROTATE_270:
            // 2x4: 270度回転（縦長）
            for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 2; col++) {
                    positions.push_back(sf::Vector2i(
                        basePos.x + col * tileSize,
                        basePos.y + row * tileSize
                    ));
                }
            }
            break;
        }

        return positions;
    }
};

/**
 * 大型タイルツール（回転対応版）
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
     * 位置を大型タイルサイズとブラシサイズにスナップ
     * @param pos 元の位置
     * @param view CanvasViewインスタンス
     * @param screenTileSize スクリーン上のタイルサイズ
     * @param brushSize ブラシサイズ
     * @param canvasTileSize キャンバスの実際のタイルサイズ
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

    /**
     * 回転インジケーターを描画
     */
    void drawRotationIndicator(sf::RenderWindow& window,
        const sf::Vector2i& center,
        RotationAngle rotation) const;
};

/**
 * 大型タイル管理クラス（回転対応版）
 * 大型タイルの選択と情報管理
 */
class LargeTileManager {
private:
    std::vector<LargeTile> largeTiles;
    int currentSelection = 0;
    LargeTileRotation globalRotation; // 全体の回転状態

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
    LargeTile& getCurrentLargeTile() {
        return largeTiles[currentSelection];
    }

    const LargeTile& getCurrentLargeTile() const {
        return largeTiles[currentSelection];
    }

    /**
     * 指定IDの大型タイルを取得
     */
    const LargeTile& getLargeTile(int id) const {
        if (id >= 0 && id < 12) {
            return largeTiles[id];
        }
        return largeTiles[0];  // フォールバック
    }

    /**
     * 現在選択中の大型タイルを回転
     */
    void rotateCurrentTile() {
        largeTiles[currentSelection].rotateNext();
    }

    /**
     * 全体回転を次に進める
     */
    void rotateGlobal() {
        globalRotation.rotateNext();
        // 全ての大型タイルに回転を適用
        for (auto& tile : largeTiles) {
            tile.setRotation(globalRotation.getCurrentRotation());
        }
    }

    /**
     * 現在の回転状態を取得
     */
    RotationAngle getCurrentRotation() const {
        return largeTiles[currentSelection].getCurrentRotation();
    }

    int getCurrentRotationDegrees() const {
        return static_cast<int>(getCurrentRotation());
    }

    /**
     * 回転をリセット
     */
    void resetRotation() {
        globalRotation.reset();
        for (auto& tile : largeTiles) {
            tile.setRotation(RotationAngle::ROTATE_0);
        }
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