#pragma once
//===== LargeTilePaletteOverlay.hpp =====
#pragma once
#include <SFML/Graphics.hpp>
#include "LargeTileSystem.hpp"

/**
 * 大型タイルパレットオーバーレイ
 * 既存のTilePaletteの上に大型タイル選択領域を表示
 */
class LargeTilePaletteOverlay {
private:
    sf::Vector2f palettePosition;    // TilePaletteの位置
    float tileSize;                  // 単一タイルのサイズ
    int tilesPerRow;                 // 1行あたりのタイル数（4固定）
    bool isVisible = false;          // オーバーレイの表示状態
    int selectedLargeTile = -1;      // 選択中の大型タイル番号

    // 大型タイル領域の定義
    struct LargeTileRegion {
        sf::Vector2i topLeft;        // 左上タイル座標（行,列）
        sf::Vector2i size;           // サイズ（行数,列数）
        int largeTileId;             // 大型タイル番号
    };

    std::vector<LargeTileRegion> largeTileRegions;

public:
    /**
     * コンストラクタ
     * @param pos TilePaletteの位置（TilePalette::positionと同じ）
     * @param tSize タイルサイズ（TilePalette::tileSizeと同じ）
     */
    LargeTilePaletteOverlay(const sf::Vector2f& pos, float tSize)
        : palettePosition(pos), tileSize(tSize), tilesPerRow(4) {
        initializeLargeTileRegions();
    }

    /**
     * 大型タイル領域を初期化
     * 配置表に基づいて各大型タイルの領域を定義
     */
    void initializeLargeTileRegions() {
        largeTileRegions.clear();

        // 大型タイル 0-7（2x2パターン）
        for (int i = 0; i < 8; ++i) {
            LargeTileRegion region;
            region.largeTileId = i;
            region.size = sf::Vector2i(2, 2);  // 2行x2列

            // 配置計算：0,1は0行目、2,3は2行目...
            int baseRow = (i / 2) * 2;  // 0,0,2,2,4,4,6,6
            int baseCol = (i % 2) * 2;  // 0,2,0,2,0,2,0,2

            region.topLeft = sf::Vector2i(baseRow, baseCol);
            largeTileRegions.push_back(region);
        }

        // 大型タイル 8-11（4x2パターン）
        for (int i = 8; i < 12; ++i) {
            LargeTileRegion region;
            region.largeTileId = i;
            region.size = sf::Vector2i(2, 4);  // 2行x4列

            int baseRow = 8 + (i - 8) * 2;  // 8,10,12,14
            int baseCol = 0;

            region.topLeft = sf::Vector2i(baseRow, baseCol);
            largeTileRegions.push_back(region);
        }
    }

    /**
     * オーバーレイの表示/非表示を切り替え
     */
    void setVisible(bool visible) {
        isVisible = visible;
        if (!visible) {
            selectedLargeTile = -1;  // 非表示時は選択解除
        }
    }

    /**
     * オーバーレイが表示中かチェック
     */
    bool getVisible() const {
        return isVisible;
    }

    /**
     * マウスクリック処理
     * @param mousePos マウス座標
     * @return クリックされた大型タイル番号（-1: クリックなし）
     */
    int handleClick(const sf::Vector2i& mousePos) {
        if (!isVisible) return -1;

        // マウス座標をタイル座標に変換
        sf::Vector2f localPos = static_cast<sf::Vector2f>(mousePos) - palettePosition;

        // タイル座標を計算（5ピクセルの間隔を考慮）
        int col = static_cast<int>(localPos.x / (tileSize + 5));
        int row = static_cast<int>(localPos.y / (tileSize + 5));

        // 各大型タイル領域をチェック
        for (const auto& region : largeTileRegions) {
            if (isInRegion(row, col, region)) {
                selectedLargeTile = region.largeTileId;
                return region.largeTileId;
            }
        }

        return -1;  // どの領域にも該当しない
    }

    /**
     * オーバーレイ描画
     * @param window 描画ウィンドウ
     */
    void draw(sf::RenderWindow& window) const {
        if (!isVisible) return;

        // 半透明オーバーレイ背景
        sf::RectangleShape overlay(sf::Vector2f(
            tilesPerRow * (tileSize + 5) - 5,  // 4列分の幅
            16 * (tileSize + 5) - 5            // 16行分の高さ
        ));
        overlay.setPosition(palettePosition);
        overlay.setFillColor(sf::Color(0, 0, 0, 120));  // 半透明黒
        window.draw(overlay);

        // 各大型タイル領域を描画
        for (const auto& region : largeTileRegions) {
            drawLargeTileRegion(window, region);
        }

        // 選択中の大型タイルをハイライト
        if (selectedLargeTile >= 0) {
            for (const auto& region : largeTileRegions) {
                if (region.largeTileId == selectedLargeTile) {
                    drawSelectionHighlight(window, region);
                    break;
                }
            }
        }
    }

    /**
     * 現在選択中の大型タイル番号を取得
     */
    int getSelectedLargeTile() const {
        return selectedLargeTile;
    }

    /**
     * 大型タイル番号を直接設定
     */
    void setSelectedLargeTile(int largeTileId) {
        if (largeTileId >= 0 && largeTileId < 12) {
            selectedLargeTile = largeTileId;
        }
    }

private:
    /**
     * 指定座標が大型タイル領域内かチェック
     */
    bool isInRegion(int row, int col, const LargeTileRegion& region) const {
        return (row >= region.topLeft.x &&
            row < region.topLeft.x + region.size.x &&
            col >= region.topLeft.y &&
            col < region.topLeft.y + region.size.y);
    }

    /**
     * 大型タイル領域を描画
     */
    void drawLargeTileRegion(sf::RenderWindow& window, const LargeTileRegion& region) const {
        // 領域の境界線を描画
        float regionWidth = region.size.y * (tileSize + 5) - 5;
        float regionHeight = region.size.x * (tileSize + 5) - 5;

        sf::Vector2f regionPos(
            palettePosition.x + region.topLeft.y * (tileSize + 5),
            palettePosition.y + region.topLeft.x * (tileSize + 5)
        );

        sf::RectangleShape regionRect(sf::Vector2f(regionWidth, regionHeight));
        regionRect.setPosition(regionPos);
        regionRect.setFillColor(sf::Color::Transparent);
        regionRect.setOutlineThickness(2);
        regionRect.setOutlineColor(sf::Color(255, 255, 0, 150));  // 半透明黄色
        window.draw(regionRect);

        // 大型タイル番号を表示
        sf::CircleShape idMarker(8);
        idMarker.setOrigin(8, 8);
        idMarker.setPosition(
            regionPos.x + regionWidth / 2,
            regionPos.y + regionHeight / 2
        );
        idMarker.setFillColor(sf::Color(255, 0, 0, 180));
        idMarker.setOutlineThickness(1);
        idMarker.setOutlineColor(sf::Color::White);
        window.draw(idMarker);

        // 番号テキスト（簡略化 - 実際にはsf::Textを使用）
        // 実装時にはフォントが必要
    }

    /**
     * 選択ハイライトを描画
     */
    void drawSelectionHighlight(sf::RenderWindow& window, const LargeTileRegion& region) const {
        float regionWidth = region.size.y * (tileSize + 5) - 5;
        float regionHeight = region.size.x * (tileSize + 5) - 5;

        sf::Vector2f regionPos(
            palettePosition.x + region.topLeft.y * (tileSize + 5),
            palettePosition.y + region.topLeft.x * (tileSize + 5)
        );

        sf::RectangleShape highlight(sf::Vector2f(regionWidth, regionHeight));
        highlight.setPosition(regionPos);
        highlight.setFillColor(sf::Color(255, 255, 0, 80));  // 半透明黄色ハイライト
        highlight.setOutlineThickness(3);
        highlight.setOutlineColor(sf::Color::Yellow);
        window.draw(highlight);
    }
};