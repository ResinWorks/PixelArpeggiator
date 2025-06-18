#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <SFML/Graphics.hpp>
#include "PatternGrid.hpp"
#include "ColorPanel.hpp"
#include "UIHelper.hpp"
#include "TilePalette.hpp"
#include "Canvas.hpp"
#include "Button.hpp"
#include "SaveLoad.hpp"
#include "CanvasView.hpp"
#include "DrawingManager.hpp"     // 新しいDrawingManager
#include "DrawingTools.hpp"       // ツールシステム
#include "tinyfiledialogs.h"
#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include "LargeTileSystem.hpp"
#include "LargeTilePaletteOverlay.hpp"
#include <algorithm>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

/**
 * UIManagerクラス（ツール切り替えボタン追加版）
 * 従来のUIに加えて、新しい描画ツールの切り替えボタンを追加
 */
class UIManager {
private:
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<sf::RectangleShape> sliders;
    std::vector<sf::CircleShape> knobs;
    std::vector<sf::Text> labels;

public:
    UIManager(const sf::Font& font) {
        initializeButtons();
        initializeSliders(font);
    }

    void initializeButtons() {
        // ===== 既存のボタン =====
        buttons.emplace_back(std::make_unique<Button>("Add to Palette", sf::Vector2f(20, 450), sf::Vector2f(100, 25)));
        buttons.emplace_back(std::make_unique<Button>("Save Changes", sf::Vector2f(150, 450), sf::Vector2f(100, 25)));
        // ===== ファイル操作ボタン（画像出力追加） =====
        buttons.emplace_back(std::make_unique<Button>("Save to File", sf::Vector2f(20, 600), sf::Vector2f(100, 30)));
        buttons.emplace_back(std::make_unique<Button>("Load from File", sf::Vector2f(130, 600), sf::Vector2f(100, 30)));
        buttons.emplace_back(std::make_unique<Button>("Export PNG", sf::Vector2f(240, 600), sf::Vector2f(100, 30))); // 新規追加
        buttons.emplace_back(std::make_unique<Button>("Export JPG", sf::Vector2f(200, 850), sf::Vector2f(100, 30))); // 新規追加

           // ===== グリッド表示切り替えボタン =====
        buttons.emplace_back(std::make_unique<Button>("Toggle Grid", sf::Vector2f(20, 640), sf::Vector2f(120, 30)));

        // ===== ブラシサイズボタン =====
        buttons.emplace_back(std::make_unique<Button>("1x1", sf::Vector2f(20, 690), sf::Vector2f(50, 30)));
        buttons.emplace_back(std::make_unique<Button>("3x3", sf::Vector2f(80, 690), sf::Vector2f(50, 30)));
        buttons.emplace_back(std::make_unique<Button>("5x5", sf::Vector2f(140, 690), sf::Vector2f(50, 30)));

        // ===== ツール切り替えボタン（新設計） =====
        buttons.emplace_back(std::make_unique<Button>("Brush", sf::Vector2f(20, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Eraser", sf::Vector2f(80, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Line", sf::Vector2f(140, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Circle", sf::Vector2f(200, 730), sf::Vector2f(60, 30))); // 新規追加
        buttons.emplace_back(std::make_unique<Button>("Ellipse", sf::Vector2f(260, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Large", sf::Vector2f(200, 690), sf::Vector2f(60, 30))); // 新規
        // ===== 新規追加：大型タイルパレット表示切り替えボタン =====
        buttons.emplace_back(std::make_unique<Button>("L-Palette", sf::Vector2f(250, 690), sf::Vector2f(80, 30))); // 新規追加


        // ===== ビュー関連ボタン =====
        buttons.emplace_back(std::make_unique<Button>("Reset View", sf::Vector2f(20, 770), sf::Vector2f(100, 30)));
    }

    void initializeSliders(const sf::Font& font) {

        // タイルグリッド線調整スライダー
        /*
        */
        sliders.emplace_back(sf::Vector2f(120, 10));
        sliders.emplace_back(sf::Vector2f(120, 10));
        sliders[0].setPosition(70, 550);
        sliders[1].setPosition(70, 580);
        sliders[0].setFillColor(sf::Color(80, 80, 80));
        sliders[1].setFillColor(sf::Color(80, 80, 80));

        // ===== 新規追加：タイル内部グリッド色スライダー =====
        sliders.emplace_back(sf::Vector2f(100, 8));  // Red
        sliders.emplace_back(sf::Vector2f(100, 8));  // Green
        sliders.emplace_back(sf::Vector2f(100, 8));  // Blue

        sliders[2].setPosition(240, 550);  // Red
        sliders[3].setPosition(240, 570);  // Green
        sliders[4].setPosition(240, 590);  // Blue

        sliders[2].setFillColor(sf::Color(100, 50, 50));   // 赤っぽい背景
        sliders[3].setFillColor(sf::Color(50, 100, 50));   // 緑っぽい背景
        sliders[4].setFillColor(sf::Color(50, 50, 100));   // 青っぽい背景


        knobs.emplace_back(5);
        knobs.emplace_back(5);
        knobs[0].setFillColor(sf::Color::White);
        knobs[1].setFillColor(sf::Color::White);

        // ===== 新規追加：RGB ノブ =====
        knobs.emplace_back(4);  // Red
        knobs.emplace_back(4);  // Green
        knobs.emplace_back(4);  // Blue
        knobs[2].setFillColor(sf::Color::Red);
        knobs[3].setFillColor(sf::Color::Green);
        knobs[4].setFillColor(sf::Color::Blue);

        labels.emplace_back("Spacing", font, 14);
        labels.emplace_back("Shrink", font, 14);
        labels[0].setPosition(20, 545);
        labels[1].setPosition(20, 575);

        // ===== 新規追加：タイル内部グリッド色ラベル =====
        labels.emplace_back("Tile Grid Color", font, 12);
        labels.emplace_back("R", font, 11);
        labels.emplace_back("G", font, 11);
        labels.emplace_back("B", font, 11);

        labels[2].setPosition(220, 530);   // メインラベル
        labels[3].setPosition(220, 547);   // R
        labels[4].setPosition(220, 567);   // G
        labels[5].setPosition(220, 587);   // B

    }



    // ボタンアクセス用のenum（ツール対応版）
    enum ButtonIndex {
        ADD_PATTERN = 0,
        SAVE_CHANGES = 1,
        SAVE_FILE = 2,
        LOAD_FILE = 3,
        EXPORT_PNG = 4,      // 新規追加
        EXPORT_JPG = 5,      // 新規追加
        TOGGLE_GRID = 6,
        BRUSH_SMALL = 7,
        BRUSH_MEDIUM = 8,
        BRUSH_LARGE = 9,
        TOOL_BRUSH = 10,      // 新規追加：ブラシツール
        TOOL_ERASER = 11,     // 新規追加：消しゴムツール
        TOOL_LINE = 12,      // 新規追加：直線ツール
        TOOL_CIRCLE = 13,    // 新規追加
        TOOL_ELLIPSE = 14,   // 新規追加：楕円ツール
        TOOL_LARGE_TILE = 15,  // 新規追加
        LARGE_TILE_PALETTE_TOGGLE = 16,  // 新規追加
        RESET_VIEW = 17      // インデックス更新
    };

    Button& getButton(ButtonIndex index) {
        return *buttons[index];
    }

    void updateButtons(const sf::Vector2i& mousePos) {
        for (auto& button : buttons) {
            button->update(mousePos);
        }
    }

    /**
     * ボタン描画（アクティブツール強調表示付き）
     * @param window 描画ウィンドウ
     * @param font フォント
     * * @param currentBrushSize 現在のブラシサイズ
     * @param activeToolType 現在アクティブなツールタイプ
     */
    void drawButtons(sf::RenderWindow& window, const sf::Font& font, ToolManager::ToolType activeToolType, int currentBrushSize,
        bool largeTilePaletteVisible = false)
    {
        for (size_t i = 0; i < buttons.size(); ++i) {
            // ツールボタンの場合、アクティブ状態を考慮
            bool isActive = false;
            if (i == TOOL_BRUSH && activeToolType == ToolManager::ToolType::BRUSH) isActive = true;
            if (i == TOOL_ERASER && activeToolType == ToolManager::ToolType::ERASER) isActive = true;
            if (i == TOOL_LINE && activeToolType == ToolManager::ToolType::LINE) isActive = true;
            if (i == TOOL_CIRCLE && activeToolType == ToolManager::ToolType::CIRCLE) isActive = true;
            if (i == TOOL_ELLIPSE && activeToolType == ToolManager::ToolType::ELLIPSE) isActive = true; // 更新
            if (i == TOOL_LARGE_TILE && activeToolType == ToolManager::ToolType::LARGE_TILE) isActive = true; // 新規追加

            // 大型タイルパレット表示ボタンのアクティブ状態
            if (i == LARGE_TILE_PALETTE_TOGGLE && largeTilePaletteVisible) isActive = true;

            // ブラシサイズボタンのアクティブ状態チェック
            if (i == BRUSH_SMALL && currentBrushSize == 1) isActive = true;
            if (i == BRUSH_MEDIUM && currentBrushSize == 3) isActive = true;
            if (i == BRUSH_LARGE && currentBrushSize == 5) isActive = true;

            // アクティブなツールボタンは色を変える
            if (isActive) {
                // アクティブ時の強調表示（一時的に色を変更）
                buttons[i]->setActiveState(true);
            }

            buttons[i]->draw(window, font);

            // 元の状態に戻す
            if (isActive) {
                buttons[i]->setActiveState(false);
            }
        }
    }

    void updateSliders(const sf::Vector2i& mousePos, bool mousePressed,
        float& gridSpacing, float& gridShrink,
        sf::Color& tileGridColor) 
    {
        // タイルグリッド調整スライダー
        //1ピクセル１色対応なら表示しない
        /*
        */
        sf::FloatRect spacingBounds = sliders[0].getGlobalBounds();
        if (mousePressed && spacingBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            float ratio = (mousePos.x - spacingBounds.left) / spacingBounds.width;
            gridSpacing = std::clamp(ratio, 0.0f, 1.0f);
        }

        sf::FloatRect shrinkBounds = sliders[1].getGlobalBounds();
        if (mousePressed && shrinkBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            float ratio = (mousePos.x - shrinkBounds.left) / shrinkBounds.width;
            gridShrink = std::clamp(ratio, 0.0f, 1.0f);
        }

        // ===== 新規追加：RGB スライダー処理 =====
       // Red スライダー
        sf::FloatRect redBounds = sliders[2].getGlobalBounds();
        if (mousePressed && redBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            float ratio = (mousePos.x - redBounds.left) / redBounds.width;
            sf::Uint8 redValue = static_cast<sf::Uint8>(ratio * 255);
            tileGridColor.r = redValue;
        }

        // Green スライダー
        sf::FloatRect greenBounds = sliders[3].getGlobalBounds();
        if (mousePressed && greenBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            float ratio = (mousePos.x - greenBounds.left) / greenBounds.width;
            sf::Uint8 greenValue = static_cast<sf::Uint8>(ratio * 255);
            tileGridColor.g = greenValue;
        }

        // Blue スライダー
        sf::FloatRect blueBounds = sliders[4].getGlobalBounds();
        if (mousePressed && blueBounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            float ratio = (mousePos.x - blueBounds.left) / blueBounds.width;
            sf::Uint8 blueValue = static_cast<sf::Uint8>(ratio * 255);
            tileGridColor.b = blueValue;
        }
    }

    void drawSliders(sf::RenderWindow& window, float gridSpacing, float gridShrink,
        const sf::Color& tileGridColor)
    {
        // 既存のスライダー描画（変更なし）
        for (auto& label : labels) {
            window.draw(label);
        }

        for (auto& slider : sliders) {
            window.draw(slider);
        }

        knobs[0].setPosition(
            sliders[0].getPosition().x + sliders[0].getSize().x * gridSpacing - 5,
            sliders[0].getPosition().y - 3
        );
        knobs[1].setPosition(
            sliders[1].getPosition().x + sliders[1].getSize().x * gridShrink - 5,
            sliders[1].getPosition().y - 3
        );

        // ===== 新規追加：RGB ノブ位置更新・描画 =====
        float redRatio = tileGridColor.r / 255.0f;
        float greenRatio = tileGridColor.g / 255.0f;
        float blueRatio = tileGridColor.b / 255.0f;

        knobs[2].setPosition(
            sliders[2].getPosition().x + sliders[2].getSize().x * redRatio - 4,
            sliders[2].getPosition().y - 2
        );
        knobs[3].setPosition(
            sliders[3].getPosition().x + sliders[3].getSize().x * greenRatio - 4,
            sliders[3].getPosition().y - 2
        );
        knobs[4].setPosition(
            sliders[4].getPosition().x + sliders[4].getSize().x * blueRatio - 4,
            sliders[4].getPosition().y - 2
        );

        // ===== 全ノブ描画 =====
        for (auto& knob : knobs) {
            window.draw(knob);
        }

        // ===== 色プレビュー表示 =====
        sf::RectangleShape colorPreview(sf::Vector2f(30, 10));
        colorPreview.setPosition(310, 530);
        colorPreview.setFillColor(tileGridColor);
        colorPreview.setOutlineThickness(1);
        colorPreview.setOutlineColor(sf::Color::White);
        window.draw(colorPreview);

        // RGB 値表示
        std::string rgbText = "(" + std::to_string(tileGridColor.r) + "," +
            std::to_string(tileGridColor.g) + "," +
            std::to_string(tileGridColor.b) + ")";
        // テキスト表示は簡略化（実際には sf::Text を使用）
    }

   
};



/**
* 画像出力用のヘルパー関数
* ファイル名の生成とダイアログ表示
*/
class ImageExportHelper {
public:
    /**
     * 現在の日時を使ってデフォルトファイル名を生成
     * @param extension ファイル拡張子（"png", "jpg" など）
     * @return 生成されたファイル名
     */
    static std::string generateDefaultFilename(const std::string& extension) {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);

        std::ostringstream oss;
        oss << "dotarp_canvas_"
            << std::put_time(&tm, "%Y%m%d_%H%M%S")
            << "." << extension;
        return oss.str();
    }

    /**
     * ファイル保存ダイアログを表示
     * @param title ダイアログタイトル
     * @param defaultName デフォルトファイル名
     * @param extension ファイル拡張子
     * @return 選択されたファイルパス（キャンセル時は空文字列）
     */
    static std::string showSaveDialog(const std::string& title,
        const std::string& defaultName,
        const std::string& extension) {

        // ファイルフィルター設定
        std::vector<const char*> filterPatterns;
        std::string filterDescription;

        if (extension == "png") {
            filterPatterns = { "*.png" };
            filterDescription = "PNG Images";
        }
        else if (extension == "jpg" || extension == "jpeg") {
            filterPatterns = { "*.jpg", "*.jpeg" };
            filterDescription = "JPEG Images";
        }
        else {
            filterPatterns = { "*.*" };
            filterDescription = "All Files";
        }

        const char* savePath = tinyfd_saveFileDialog(
            title.c_str(),
            defaultName.c_str(),
            filterPatterns.size(),
            filterPatterns.data(),
            filterDescription.c_str()
        );

        return savePath ? std::string(savePath) : std::string();
    }

    /**
     * 高解像度出力オプションダイアログ
     * @return {幅, 高さ, キャンセルされたか}
     */
    static std::tuple<int, int, bool> showResolutionDialog() {
        // 簡単な解像度選択（実際の実装では更に詳細なダイアログが可能）
        const char* options[] = {
            "900x900 (Original)",
            "1800x1800 (2x)",
            "3600x3600 (4x)",
            "7200x7200 (8x)",
            "Custom..."
        };

        int choice = tinyfd_messageBox(
            "Export Resolution",
            "Select export resolution:",
            "info",
            "question",
            1
        );

        // 簡略化された実装（実際にはより詳細な選択UI が必要）
        switch (choice) {
        case 0: return { 900, 900, false };
        case 1: return { 1800, 1800, false };
        case 2: return { 3600, 3600, false };
        case 3: return { 7200, 7200, false };
        default: return { 900, 900, true }; // キャンセル
        }
    }
};





int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "PixelArpeggiator v3.0 ");
    window.setFramerateLimit(120);

    sf::Font font;
    if (!font.loadFromFile("ARIAL.TTF")) {

        std::cerr << "フォント読み込み失敗" << std::endl; 
    return -1;
    }

    // ===== 既存のコンポーネント =====
    PatternGrid patternGrid(3, 3, 50);
    ColorPanel colorPanel(font);
    TilePalette tilePalette(50, sf::Vector2f(20, 60));
    Canvas canvas(160, 160, 3, sf::Vector2f(360, 20));
    CanvasView canvasView(sf::Vector2f(360, 20), window.getSize());

    // ===== 新しいツールシステム =====
    DrawingManager drawingManager;  // 新しいDrawingManager

    // UI components
    UIManager uiManager(font);

    // Position setup（変更なし）
    patternGrid.setPosition(sf::Vector2f(20, 150));
    colorPanel.setPosition(sf::Vector2f(20, 310));
    tilePalette.setPosition(sf::Vector2f(1350, 20));

    // State variables（変更なし）
    int selectedColorIndex = 0;
    int brushSize = 1;
    bool showGrid = true;
    float gridSpacing = 0.0f;
    float gridShrink = 1.0f;
    bool patternChanged = false;

    // パン操作用の変数
    bool isPanning = false;
    sf::Vector2i lastPanPos;

    // ===== 新規追加：タイル内部グリッド色の状態変数 =====
   // sf::Color tileGridColor(128, 128, 128); // デフォルト：中間グレー
    sf::Color tileGridColor(0, 0, 0); // デフォルト：中間グレー

    // ===== 新規追加：大型タイルパレットオーバーレイ =====
    LargeTilePaletteOverlay largeTilePaletteOverlay(sf::Vector2f(1350, 20), 50);  // TilePaletteと同じ座標・サイズ
    // ===== 新規追加：大型タイル管理 =====
      LargeTileManager largeTileManager;
    // ===== 簡易設定：大型タイル番号をコードで指定 =====
    
    int currentLargeTileId = 0;  // 0-11で指定（後でUI化可能）


    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // ===== マウスホイールズーム =====
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (canvas.containsInView(canvasView, mousePos)) {
                    canvasView.handleMouseWheel(event.mouseWheelScroll.delta, mousePos);
                }
            }

            // ウィンドウサイズ変更
            if (event.type == sf::Event::Resized) {
                canvasView.updateWindowSize(sf::Vector2u(event.size.width, event.size.height));
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i clickPos(event.mouseButton.x, event.mouseButton.y);


                // ===== 新規追加：大型タイルパレット表示切り替え =====
                if (uiManager.getButton(UIManager::LARGE_TILE_PALETTE_TOGGLE).isClicked(clickPos, true)) {
                    largeTilePaletteOverlay.setVisible(!largeTilePaletteOverlay.getVisible());
                }

                // ===== 新規追加：大型タイルパレットでの選択 =====
                if (largeTilePaletteOverlay.getVisible()) {
                    int clickedLargeTile = largeTilePaletteOverlay.handleClick(clickPos);
                    if (clickedLargeTile >= 0) {
                        currentLargeTileId = clickedLargeTile;
                        largeTileManager.selectLargeTile(currentLargeTileId);

                        // 大型タイルツールに切り替え
                        drawingManager.setTool(ToolManager::ToolType::LARGE_TILE);

                        // 大型タイルツールに番号を設定
                        if (auto* largeTileTool = dynamic_cast<LargeTileTool*>(drawingManager.getCurrentTool())) {
                            largeTileTool->setLargeTileId(currentLargeTileId);
                        }

                        // 選択後はオーバーレイを非表示（オプション）
                        largeTilePaletteOverlay.setVisible(false);

                        continue;  // 他のクリック処理をスキップ
                    }
                }


                // ===== ツール切り替えボタン =====
                if (uiManager.getButton(UIManager::TOOL_BRUSH).isClicked(clickPos, true)) {
                    drawingManager.setTool(ToolManager::ToolType::BRUSH);
                    largeTilePaletteOverlay.setVisible(false);  // 他ツール選択時はオーバーレイ非表示
                }
                if (uiManager.getButton(UIManager::TOOL_ERASER).isClicked(clickPos, true)) {
                    drawingManager.setTool(ToolManager::ToolType::ERASER);
                    largeTilePaletteOverlay.setVisible(false);  // 他ツール選択時はオーバーレイ非表示
                }
                if (uiManager.getButton(UIManager::TOOL_LINE).isClicked(clickPos, true)) {
                    drawingManager.setTool(ToolManager::ToolType::LINE);
                    largeTilePaletteOverlay.setVisible(false);  // 他ツール選択時はオーバーレイ非表示
                }
                if (uiManager.getButton(UIManager::TOOL_CIRCLE).isClicked(clickPos, true)) {  // 新規追加
                    drawingManager.setTool(ToolManager::ToolType::CIRCLE);
                    largeTilePaletteOverlay.setVisible(false);  // 他ツール選択時はオーバーレイ非表示
                }
                if (uiManager.getButton(UIManager::TOOL_ELLIPSE).isClicked(clickPos, true)) {  // 更新
                    drawingManager.setTool(ToolManager::ToolType::ELLIPSE);
                    largeTilePaletteOverlay.setVisible(false);  // 他ツール選択時はオーバーレイ非表示
                }
                // ===== ツール切り替えボタン（大型タイル追加） =====
                if (uiManager.getButton(UIManager::TOOL_LARGE_TILE).isClicked(clickPos, true)) {
                    drawingManager.setTool(ToolManager::ToolType::LARGE_TILE);

                    // 大型タイルツールに大型タイル番号を設定
                    if (auto* largeTileTool = dynamic_cast<LargeTileTool*>(drawingManager.getCurrentTool())) {
                        largeTileTool->setLargeTileId(currentLargeTileId);
                    }
                }
                // 既存のツール切り替え...
                if (uiManager.getButton(UIManager::TOOL_BRUSH).isClicked(clickPos, true)) {
                    drawingManager.setTool(ToolManager::ToolType::BRUSH);
                }


                // ===== 既存のボタン処理 =====
                if (uiManager.getButton(UIManager::RESET_VIEW).isClicked(clickPos, true)) {
                    canvasView.reset();
                }


            

                // ブラシサイズボタン
                if (uiManager.getButton(UIManager::BRUSH_SMALL).isClicked(clickPos, true)) brushSize = 1;
                if (uiManager.getButton(UIManager::BRUSH_MEDIUM).isClicked(clickPos, true)) brushSize = 3;
                if (uiManager.getButton(UIManager::BRUSH_LARGE).isClicked(clickPos, true)) brushSize = 5;

                if (uiManager.getButton(UIManager::TOGGLE_GRID).isClicked(clickPos, true)) {
                    showGrid = !showGrid;
                }

                if (uiManager.getButton(UIManager::ADD_PATTERN).isClicked(clickPos, true)) {
                    tilePalette.addPattern(patternGrid.getTiles(), colorPanel.getColorSet());
                }

                if (uiManager.getButton(UIManager::SAVE_CHANGES).isClicked(clickPos, true) &&
                    tilePalette.getSelectedIndex() >= 0) {
                    tilePalette.updatePattern(tilePalette.getSelectedIndex(), patternGrid.getTiles());
                    tilePalette.updateColorSet(tilePalette.getSelectedIndex(), colorPanel.getColorSet());
                }

                // ファイル操作ボタン（変更なし）
                if (uiManager.getButton(UIManager::SAVE_FILE).isClicked(clickPos, true)) {
                    const char* savePath = tinyfd_saveFileDialog("Save Project", "project.dat", 0, nullptr, nullptr);
                    if (savePath) {
                        saveProject(savePath, tilePalette.getAllPatterns(),
                            tilePalette.getAllColorPalettes(), canvas.getTileIndices());
                    }
                }

                if (uiManager.getButton(UIManager::LOAD_FILE).isClicked(clickPos, true)) {
                    const char* loadPath = tinyfd_openFileDialog("Open Project", "", 0, nullptr, nullptr, 0);
                    if (loadPath) {
                        std::vector<std::vector<int>> patterns;
                        std::vector<std::array<sf::Color, 3>> colorSets;
                        std::vector<std::vector<int>> tileData;

                        if (loadProject(loadPath, patterns, colorSets, tileData)) {
                            tilePalette.loadPatterns(patterns, colorSets);
                            canvas.setTileIndices(tileData);
                            tilePalette.selectPattern(0);
                            colorPanel.setTarget(colorSets[0]);

                            auto flat = tilePalette.getPattern(0);
                            std::vector<std::vector<int>> grid(3, std::vector<int>(3));
                            for (int i = 0; i < 9; ++i) {
                                grid[i / 3][i % 3] = flat[i];
                            }
                            patternGrid.setTiles(grid);
                        }
                    }
                }
                // ===== 画像出力ボタン処理 =====
                if (uiManager.getButton(UIManager::EXPORT_PNG).isClicked(clickPos, true)) {
                    std::string defaultName = ImageExportHelper::generateDefaultFilename("png");
                    std::string savePath = ImageExportHelper::showSaveDialog(
                        "Export PNG Image", defaultName, "png"
                    );

                    if (!savePath.empty()) {
                        // 拡張子チェック・追加
                        if (savePath.substr(savePath.length() - 4) != ".png") {
                            savePath += ".png";
                        }

                        bool success = canvas.exportToImage(
                            savePath,
                            tilePalette.getAllPatterns(),
                            tilePalette.getAllColorPalettes(),
                            showGrid,
                            gridSpacing,
                            gridShrink
                        );

                        if (success) {
                            tinyfd_messageBox("Export Complete",
                                ("PNG image exported successfully:\n" + savePath).c_str(),
                                "info", "ok", 1);
                        }
                        else {
                            tinyfd_messageBox("Export Failed",
                                "Failed to export PNG image.", "error", "ok", 1);
                        }
                    }
                }

                if (uiManager.getButton(UIManager::EXPORT_JPG).isClicked(clickPos, true)) {
                    std::string defaultName = ImageExportHelper::generateDefaultFilename("jpg");
                    std::string savePath = ImageExportHelper::showSaveDialog(
                        "Export JPG Image", defaultName, "jpg"
                    );

                    if (!savePath.empty()) {
                        // 拡張子チェック・追加
                        if (savePath.substr(savePath.length() - 4) != ".jpg" &&
                            savePath.substr(savePath.length() - 5) != ".jpeg") {
                            savePath += ".jpg";
                        }

                        bool success = canvas.exportToImage(
                            savePath,
                            tilePalette.getAllPatterns(),
                            tilePalette.getAllColorPalettes(),
                            false, // JPGは背景透明度をサポートしないため、グリッドは非表示推奨
                            gridSpacing,
                            gridShrink
                        );

                        if (success) {
                            tinyfd_messageBox("Export Complete",
                                ("JPG image exported successfully:\n" + savePath).c_str(),
                                "info", "ok", 1);
                        }
                        else {
                            tinyfd_messageBox("Export Failed",
                                "Failed to export JPG image.", "error", "ok", 1);
                        }
                    }
                }


                // 中ボタンでパン開始
                if (event.mouseButton.button == sf::Mouse::Middle) {
                    isPanning = true;
                    lastPanPos = mousePos;
                }
                /*
                // Tile palette handling（変更なし）
                if (tilePalette.handleClick(clickPos)) {
                    int selIdx = tilePalette.getSelectedIndex();
                    if (selIdx >= 0) {
                        colorPanel.setTarget(tilePalette.getSelectedColorSet());
                        auto flat = tilePalette.getPattern(selIdx);
                        std::vector<std::vector<int>> grid(3, std::vector<int>(3));
                        for (int i = 0; i < 9; ++i) {
                            grid[i / 3][i % 3] = flat[i];
                        }
                        patternGrid.setTiles(grid);
                    }
                }
                */


                // 既存のTilePalette処理
                if (!largeTilePaletteOverlay.getVisible()) {  // オーバーレイ非表示時のみ
                    if (tilePalette.handleClick(clickPos)) {
                        int selIdx = tilePalette.getSelectedIndex();
                        if (selIdx >= 0) {
                            colorPanel.setTarget(tilePalette.getSelectedColorSet());
                            auto flat = tilePalette.getPattern(selIdx);
                            std::vector<std::vector<int>> grid(3, std::vector<int>(3));
                            for (int i = 0; i < 9; ++i) {
                                grid[i / 3][i % 3] = flat[i];
                            }
                            patternGrid.setTiles(grid);
                        }
                    }
                }

                // ===== キャンバス描画開始（新しいツールシステム） =====
                if (canvas.containsInView(canvasView, clickPos)) {
                    drawingManager.startDrawing(clickPos, canvas, canvasView,
                        tilePalette.getSelectedIndex(), brushSize);
                }
            }

            // ===== マウスボタン離した時の処理 =====
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Middle) {
                    isPanning = false;
                }

                // 描画終了（新しいツールシステム）
                if (event.mouseButton.button == sf::Mouse::Left && drawingManager.getIsDrawing()) {
                    sf::Vector2i releasePos(event.mouseButton.x, event.mouseButton.y);
                    drawingManager.stopDrawing(releasePos, canvas, canvasView,
                        tilePalette.getSelectedIndex(), brushSize);
                }
            }
        }


        // ===== パン操作更新 =====
        if (isPanning && sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
            sf::Vector2f delta = static_cast<sf::Vector2f>(mousePos - lastPanPos);
            canvasView.pan(delta);
            lastPanPos = mousePos;
        }

        // ===== マウス移動更新（新しいツールシステム） =====
        if (drawingManager.getIsDrawing()) {
            drawingManager.updateMovement(mousePos, canvas, canvasView,
                tilePalette.getSelectedIndex(), brushSize);
        }

      

        // ===== 更新処理（タイル内部グリッド色対応） =====
        bool colorSliderChanged = colorPanel.updateSliders(mousePos, mousePressed);
        if (colorSliderChanged && tilePalette.getSelectedIndex() >= 0) {
            tilePalette.updateColorSet(tilePalette.getSelectedIndex(), colorPanel.getColorSet());
            canvas.setDirty(true);
        }

        if (mousePressed) {
            // Color panel interaction
            colorPanel.handleEvent(mousePos, true, selectedColorIndex);

            // Pattern grid editing
            if (patternGrid.handleClick(mousePos, selectedColorIndex)) {
                patternChanged = true;
            }
        }

        // ===== スライダー更新（RGB対応） =====
        sf::Color oldTileGridColor = tileGridColor;
        uiManager.updateSliders(mousePos, mousePressed, gridSpacing, gridShrink, tileGridColor);
        // タイル内部グリッド色が変更された場合
        if (tileGridColor.r != oldTileGridColor.r ||
            tileGridColor.g != oldTileGridColor.g ||
            tileGridColor.b != oldTileGridColor.b) {
            canvas.setTileGridColor(tileGridColor);
        }



        // Pattern change handling
        if (patternChanged && tilePalette.getSelectedIndex() >= 0) {
            tilePalette.updatePattern(tilePalette.getSelectedIndex(), patternGrid.getTiles());
            canvas.setDirty(true);
            patternChanged = false;
        }

        // UI updates
        uiManager.updateButtons(mousePos);

        // ===== 描画処理 =====
        window.clear(sf::Color(30, 30, 30));

        // ===== 情報表示 =====
        // ズーム情報
        drawText(window, font, canvasView.getStatusString(),
            14, sf::Vector2f(20, 20), sf::Color::Cyan);

        // ツール情報
        std::string toolInfo = "Tool: " + drawingManager.getCurrentToolName();
        if (drawingManager.getCurrentToolType() == ToolManager::ToolType::LARGE_TILE) {
            toolInfo += " [" + std::to_string(currentLargeTileId) + "]";
        }
        else {
            toolInfo += " | Brush Size: " + std::to_string(brushSize);
        }
        drawText(window, font, toolInfo, 14, sf::Vector2f(20, 40), sf::Color::Yellow);

        // 大型タイルツール使用時の説明
        if (drawingManager.getCurrentToolType() == ToolManager::ToolType::LARGE_TILE) {
            std::string largeTileInfo = "Large Tile: 0-9,Q,W keys to select | Current: " + std::to_string(currentLargeTileId);
            drawText(window, font, largeTileInfo, 12, sf::Vector2f(20, 80), sf::Color(255, 200, 100));

           /*
            // 大型タイルの構成タイル番号を表示
            const LargeTile& currentLargeTile = largeTileManager.getLargeTile(currentLargeTileId);
            auto arrangement = currentLargeTile.getArrangement();
            std::string compositionInfo = "Tiles: [" +
                std::to_string(arrangement.topLeft) + "," +
                std::to_string(arrangement.topRight) + "," +
                std::to_string(arrangement.bottomLeft) + "," +
                std::to_string(arrangement.bottomRight) + "]";
            drawText(window, font, compositionInfo, 12, sf::Vector2f(20, 100), sf::Color(200, 200, 255));
           */
        }


        // 操作説明
        drawText(window, font, "Mouse Wheel: Zoom | Middle Drag: Pan | Left Click: Draw",
            12, sf::Vector2f(20, 60), sf::Color(200, 200, 200));

        // 直線ツール使用時の追加説明
        if (drawingManager.getCurrentToolType() == ToolManager::ToolType::LINE) {
            drawText(window, font, "Line Tool: Click and drag to draw lines",
                12, sf::Vector2f(20, 80), sf::Color(255, 200, 100));
        }
        else if (drawingManager.getCurrentToolType() == ToolManager::ToolType::CIRCLE) {  // 新規追加
            drawText(window, font, "Circle Tool: Click center and drag to set radius",
                12, sf::Vector2f(20, 80), sf::Color(255, 200, 100));
        }
        else if (drawingManager.getCurrentToolType() == ToolManager::ToolType::ELLIPSE) {  // 更新
            drawText(window, font, "Ellipse Tool: Click and drag diagonal to draw ellipse",
                12, sf::Vector2f(20, 80), sf::Color(255, 200, 100));
        }

        // ===== コンポーネント描画 =====
        if (tilePalette.getSelectedIndex() >= 0) {
            colorPanel.setTarget(tilePalette.getSelectedColorSet());
        }

        patternGrid.draw(window, colorPanel.getColorSet());
        tilePalette.draw(window, tilePalette.getAllColorPalettes());
        colorPanel.draw(window);

        // ===== 新規追加：大型タイルパレットオーバーレイ描画 =====
        largeTilePaletteOverlay.draw(window);

        // キャンバス描画（CanvasView対応）
        canvas.drawWithView(window, canvasView,
            tilePalette.getAllPatterns(),
            tilePalette.getAllColorPalettes(),
            showGrid, gridSpacing, gridShrink);

        // UI elements（大型タイルパレット表示状態を渡す）
        uiManager.drawButtons(window, font, drawingManager.getCurrentToolType(), brushSize,
            largeTilePaletteOverlay.getVisible());
        uiManager.drawSliders(window, gridSpacing, gridShrink, tileGridColor);

        // ===== カーソル＆プレビュー描画（新しいツールシステム） =====
        if (canvas.containsInView(canvasView, mousePos)) {
            // カーソル描画（ツール別）
            drawingManager.drawCursor(window, mousePos, canvasView, brushSize, canvas.getTileSize());

            // プレビュー描画（直線ツール等）
            drawingManager.drawPreview(window, mousePos, canvasView, brushSize);
        }

        // ===== ステータス情報 =====
        drawText(window, font, "Selected Color: " + std::to_string(selectedColorIndex),
            14, sf::Vector2f(20, 840), sf::Color::White);

        if (tilePalette.getSelectedIndex() >= 0) {
            drawText(window, font, "Selected Tile: " + std::to_string(tilePalette.getSelectedIndex()),
                14, sf::Vector2f(20, 860), sf::Color::White);
        }
        else {
            drawText(window, font, "No Tile Selected", 14, sf::Vector2f(20, 860), sf::Color::White);
        }

        // キャンバス情報表示（新規追加）
        auto canvasSize = canvas.getCanvasPixelSize();
        std::string canvasInfo = "Canvas: " + std::to_string(canvasSize.first) + "x" + std::to_string(canvasSize.second) + " px";
        drawText(window, font, canvasInfo, 14, sf::Vector2f(20, 820), sf::Color(200, 200, 200));


        // 描画統計情報（デバッグ用、F1キー押下時表示）
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
            auto stats = drawingManager.getDrawingStats();
            std::string debugInfo = "Drawing: " + std::string(stats.isDrawing ? "YES" : "NO") +
                " | Moved: " + std::string(stats.hasMoved ? "YES" : "NO");
            drawText(window, font, debugInfo, 12, sf::Vector2f(20, 100), sf::Color::Green);
        }

        window.display();
    }
    /**
 * 画像出力用のヘルパー関数
 * ファイル名の生成とダイアログ表示
 */
    class ImageExportHelper {
    public:
        /**
         * 現在の日時を使ってデフォルトファイル名を生成
         * @param extension ファイル拡張子（"png", "jpg" など）
         * @return 生成されたファイル名
         */
        static std::string generateDefaultFilename(const std::string& extension) {
            auto now = std::time(nullptr);
            auto tm = *std::localtime(&now);

            std::ostringstream oss;
            oss << "dotarp_canvas_"
                << std::put_time(&tm, "%Y%m%d_%H%M%S")
                << "." << extension;
            return oss.str();
        }

        /**
         * ファイル保存ダイアログを表示
         * @param title ダイアログタイトル
         * @param defaultName デフォルトファイル名
         * @param extension ファイル拡張子
         * @return 選択されたファイルパス（キャンセル時は空文字列）
         */
        static std::string showSaveDialog(const std::string& title,
            const std::string& defaultName,
            const std::string& extension) {

            // ファイルフィルター設定
            std::vector<const char*> filterPatterns;
            std::string filterDescription;

            if (extension == "png") {
                filterPatterns = { "*.png" };
                filterDescription = "PNG Images";
            }
            else if (extension == "jpg" || extension == "jpeg") {
                filterPatterns = { "*.jpg", "*.jpeg" };
                filterDescription = "JPEG Images";
            }
            else {
                filterPatterns = { "*.*" };
                filterDescription = "All Files";
            }

            const char* savePath = tinyfd_saveFileDialog(
                title.c_str(),
                defaultName.c_str(),
                filterPatterns.size(),
                filterPatterns.data(),
                filterDescription.c_str()
            );

            return savePath ? std::string(savePath) : std::string();
        }

        /**
         * 高解像度出力オプションダイアログ
         * @return {幅, 高さ, キャンセルされたか}
         */
        static std::tuple<int, int, bool> showResolutionDialog() {
            // 簡単な解像度選択（実際の実装では更に詳細なダイアログが可能）
            const char* options[] = {
                "900x900 (Original)",
                "1800x1800 (2x)",
                "3600x3600 (4x)",
                "7200x7200 (8x)",
                "Custom..."
            };

            int choice = tinyfd_messageBox(
                "Export Resolution",
                "Select export resolution:",
                "info",
                "question",
                1
            );

            // 簡略化された実装（実際にはより詳細な選択UI が必要）
            switch (choice) {
            case 0: return { 900, 900, false };
            case 1: return { 1800, 1800, false };
            case 2: return { 3600, 3600, false };
            case 3: return { 7200, 7200, false };
            default: return { 900, 900, true }; // キャンセル
            }
        }
    };


    return 0;
}