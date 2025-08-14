#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
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
#include "DrawingManager.hpp"
#include "DrawingTools.hpp"
#include "LargeTileSystem.hpp"
#include "LargeTilePaletteOverlay.hpp"
#include "UIManager.hpp"  // 新しく分離したUIManager
#include "tinyfiledialogs.h"
#include "GlobalColorPalette.hpp"


//#include <iostream>
//#include <set>
//#include <vector>
///#include <memory>
//#include <algorithm>
//#include <ctime>
//#include <iomanip>
//#include <sstream>


extern LargeTileManager largeTileManager;

const int WINDOW_WIDTH = 1800;
const int WINDOW_HEIGHT = 900;

/**
 * 画像出力用のヘルパー関数
 */
class ImageExportHelper {
public:
    static std::string generateDefaultFilename(const std::string& extension) {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);

        std::ostringstream oss;
        oss << "dotarp_canvas_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << "." << extension;
        return oss.str();
    }

    static std::string showSaveDialog(const std::string& title, const std::string& defaultName, const std::string& extension) {
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
            title.c_str(), defaultName.c_str(),
            filterPatterns.size(), filterPatterns.data(),
            filterDescription.c_str()
        );

        return savePath ? std::string(savePath) : std::string();
    }
};

/**
 * UIManagerクラス - 分離済み（UIManager.hpp/cppに移動）
 */

// ===== 関数宣言 =====
void handleButtonClicks(const sf::Vector2i& clickPos, UIManager& uiManager,
    DrawingManager& drawingManager, LargeTilePaletteOverlay& largeTilePaletteOverlay,
    LargeTileManager& largeTileManager, int& currentLargeTileId,
    int& brushSize, bool& showGrid, TilePalette& tilePalette,
    PatternGrid& patternGrid, ColorPanel& colorPanel,
    Canvas& canvas, CanvasView& canvasView, GlobalColorPalette& globalColorPalette);

void handleFileOperations(const sf::Vector2i& clickPos, UIManager& uiManager,
    TilePalette& tilePalette, PatternGrid& patternGrid,
    ColorPanel& colorPanel, Canvas& canvas);

void exportImage(TilePalette& tilePalette, Canvas& canvas, const std::string& format);

void handleKeyboardInput(const sf::Event& event, LargeTileManager& largeTileManager,
    int& currentLargeTileId, DrawingManager& drawingManager);

void selectLargeTile(LargeTileManager& largeTileManager, DrawingManager& drawingManager,
    int& currentLargeTileId, int tileId);

void updateGameState(const sf::Vector2i& mousePos, bool mousePressed, bool& isPanning,
    sf::Vector2i& lastPanPos, CanvasView& canvasView, DrawingManager& drawingManager,
    Canvas& canvas, TilePalette& tilePalette, ColorPanel& colorPanel,
    PatternGrid& patternGrid, UIManager& uiManager, float& gridSpacing,
    float& gridShrink, sf::Color& tileGridColor, int& selectedColorIndex,
    bool& patternChanged, int brushSize, GlobalColorPalette& globalColorPalette);

void renderFrame(sf::RenderWindow& window, const sf::Font& font, PatternGrid& patternGrid,
    TilePalette& tilePalette, ColorPanel& colorPanel, Canvas& canvas,
    CanvasView& canvasView, LargeTilePaletteOverlay& largeTilePaletteOverlay,
    DrawingManager& drawingManager, UIManager& uiManager, const sf::Vector2i& mousePos,
    int selectedColorIndex, int brushSize, bool showGrid, float gridSpacing,
    float gridShrink, const sf::Color& tileGridColor, int currentLargeTileId,
    LargeTileManager& largeTileManager, GlobalColorPalette& globalColorPalette);

void renderInfoText(sf::RenderWindow& window, const sf::Font& font, CanvasView& canvasView,
    DrawingManager& drawingManager, int currentLargeTileId,
    LargeTileManager& largeTileManager, int brushSize,
    int selectedColorIndex, TilePalette& tilePalette, Canvas& canvas,
    GlobalColorPalette& globalColorPalette);

void renderToolSpecificInfo(sf::RenderWindow& window, const sf::Font& font,
    DrawingManager& drawingManager, LargeTileManager& largeTileManager,
    int currentLargeTileId);

// ユーティリティ関数
void drawText(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
    int fontSize, const sf::Vector2f& position, const sf::Color& color) {
    sf::Text sfText(text, font, fontSize);
    sfText.setPosition(position);
    sfText.setFillColor(color);
    window.draw(sfText);
}

void drawRotationGuide(sf::RenderWindow& window, const sf::Font& font, int rotationDegrees) {
    if (rotationDegrees == 0) return;

    sf::Text rotationText(std::to_string(rotationDegrees) + "°", font, 20);
    rotationText.setPosition(WINDOW_WIDTH - 60, 20);
    rotationText.setFillColor(sf::Color::Yellow);
    window.draw(rotationText);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "PixelArpeggiator v3.0");
    window.setFramerateLimit(120);

    sf::Font font;
    if (!font.loadFromFile("x14y24pxHeadUpDaisy.TTF")) {
        std::cerr << "フォント読み込み失敗" << std::endl;
        return -1;
    }

    // コンポーネント初期化
    PatternGrid patternGrid(3, 3, 50);
    ColorPanel colorPanel(font);
    TilePalette tilePalette(50, sf::Vector2f(20, 60));

    GlobalColorPalette globalColorPalette(sf::Vector2f(20, 20), 50);

    Canvas canvas(160, 160, 3, sf::Vector2f(360, 20));
    CanvasView canvasView(sf::Vector2f(360, 20), window.getSize());
    DrawingManager drawingManager;
    UIManager uiManager(font);
    LargeTilePaletteOverlay largeTilePaletteOverlay(sf::Vector2f(1350, 20), 50);

    // 位置設定
    patternGrid.setPosition(sf::Vector2f(20, 150));
    colorPanel.setPosition(sf::Vector2f(20, 310));
    tilePalette.setPosition(sf::Vector2f(1350, 20));
    globalColorPalette.setPosition(sf::Vector2f(1600, 20));

    // 新システム：ColorPanelとGlobalColorPaletteの連携設定
    colorPanel.setGlobalColorPalette(&globalColorPalette);

    // デフォルトのグローバルカラーインデックスを設定（最初の3色）
    colorPanel.setGlobalColorIndices({ 0, 1, 2 });

    // テスト用：デフォルトパターンを1つ追加
    std::vector<std::vector<int>> defaultPattern = {
        {0, 1, 0},
        {1, 2, 1},
        {0, 1, 0}
    };
    tilePalette.addPatternWithGlobalColors(defaultPattern, { 0, 1, 2 });

    // デフォルトパターンを選択
    tilePalette.selectPattern(0);
    colorPanel.setTarget(tilePalette.getSelectedColorSet());
    patternGrid.setTiles(defaultPattern);

    // 状態変数
    int selectedColorIndex = 0;
    int brushSize = 1;
    bool showGrid = true;
    float gridSpacing = 0.0f;
    float gridShrink = 1.0f;
    bool patternChanged = false;
    bool isPanning = false;
    sf::Vector2i lastPanPos;
    sf::Color tileGridColor(0, 0, 0);
    int currentLargeTileId = 0;

    // メインループ
    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // ズーム・パン処理
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (canvas.containsInView(canvasView, mousePos)) {
                    canvasView.handleMouseWheel(event.mouseWheelScroll.delta, mousePos);
                }
            }

            if (event.type == sf::Event::Resized) {
                canvasView.updateWindowSize(sf::Vector2u(event.size.width, event.size.height));
            }

            // マウスクリック処理
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i clickPos(event.mouseButton.x, event.mouseButton.y);

                // ボタン処理
                handleButtonClicks(clickPos, uiManager, drawingManager, largeTilePaletteOverlay,
                    largeTileManager, currentLargeTileId, brushSize, showGrid,
                    tilePalette, patternGrid, colorPanel, canvas, canvasView,globalColorPalette);

                // 描画開始
                if (canvas.containsInView(canvasView, clickPos)) {
                    drawingManager.startDrawing(clickPos, canvas, canvasView,
                        tilePalette.getSelectedIndex(), brushSize);
                }
            }

            // マウスリリース処理
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left && drawingManager.getIsDrawing()) {
                    sf::Vector2i releasePos(event.mouseButton.x, event.mouseButton.y);
                    drawingManager.stopDrawing(releasePos, canvas, canvasView,
                        tilePalette.getSelectedIndex(), brushSize);
                }
            }

            // 中ボタンパン処理
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
                isPanning = true;
                lastPanPos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle) {
                isPanning = false;
            }

            // キーボードショートカット
            handleKeyboardInput(event, largeTileManager, currentLargeTileId, drawingManager);
        }

        // 更新処理
        updateGameState(mousePos, mousePressed, isPanning, lastPanPos, canvasView,
            drawingManager, canvas, tilePalette, colorPanel, patternGrid,
            uiManager, gridSpacing, gridShrink, tileGridColor,
            selectedColorIndex, patternChanged, brushSize,globalColorPalette);

        // 描画処理
        renderFrame(window, font, patternGrid, tilePalette, colorPanel, canvas, canvasView,
            largeTilePaletteOverlay, drawingManager, uiManager, mousePos,
            selectedColorIndex, brushSize, showGrid, gridSpacing, gridShrink,
            tileGridColor, currentLargeTileId, largeTileManager, globalColorPalette);
    }

    return 0;
}

// ===== ヘルパー関数の実装 =====

/**
 * ボタンクリック処理
 */
void handleButtonClicks(const sf::Vector2i& clickPos, UIManager& uiManager,
    DrawingManager& drawingManager, LargeTilePaletteOverlay& largeTilePaletteOverlay,
    LargeTileManager& largeTileManager, int& currentLargeTileId,
    int& brushSize, bool& showGrid, TilePalette& tilePalette,
    PatternGrid& patternGrid, ColorPanel& colorPanel,
    Canvas& canvas, CanvasView& canvasView, GlobalColorPalette& globalColorPalette) {


    if (globalColorPalette.handleClick(clickPos)) {
        // ログ出力
        int selectedGlobalColor = globalColorPalette.getSelectedIndex();
        sf::Color selectedColor = globalColorPalette.getSelectedColor();

        // コンソールにログ出力
        std::cout << "Global Color Palette - Selected Index: " << selectedGlobalColor
            << " Color: RGB(" << (int)selectedColor.r << ", "
            << (int)selectedColor.g << ", " << (int)selectedColor.b << ")" << std::endl;
        // ColorPanelの現在選択中の色スロットにグローバルカラーを適用
        if (tilePalette.getSelectedIndex() >= 0) {

            // 新システム：選択されたグローバルカラーを現在の色スロットに設定
            colorPanel.setCurrentSlotGlobalIndex(selectedGlobalColor);
            //colorPanel.setCurrentColor(selectedColor); // 現在選択中の色スロットに適用

            // TilePaletteのグローバルカラーインデックスも更新
            auto currentIndices = colorPanel.getGlobalColorIndices();
            tilePalette.setGlobalColorIndices(tilePalette.getSelectedIndex(), currentIndices);

            // タイルパレットの色セットも更新
            tilePalette.updateColorSet(tilePalette.getSelectedIndex(), colorPanel.getColorSet());
            canvas.setDirty(true);

            std::cout << "Applied global color to current color slot in pattern" << std::endl;
        }
        else {
            std::cout << "No tile pattern selected - global color not applied" << std::endl;
        }

        return; // 他の処理を実行しないように早期リターン
    }

    // 回転ボタン
    if (uiManager.getButton(ButtonIndex::ROTATE_BUTTON).isClicked(clickPos, true)) {
        largeTileManager.rotateCurrentTile();
    }

    // 大型タイルパレット表示切り替え
    if (uiManager.getButton(ButtonIndex::LARGE_TILE_PALETTE_TOGGLE).isClicked(clickPos, true)) {
        largeTilePaletteOverlay.setVisible(!largeTilePaletteOverlay.getVisible());
    }

    // 大型タイルパレットでの選択
    if (largeTilePaletteOverlay.getVisible()) {
        int clickedLargeTile = largeTilePaletteOverlay.handleClick(clickPos);
        if (clickedLargeTile >= 0) {
            largeTileManager.selectLargeTile(clickedLargeTile);
            currentLargeTileId = clickedLargeTile;
            drawingManager.setTool(ToolManager::ToolType::LARGE_TILE);
            largeTilePaletteOverlay.setVisible(false);
            return;
        }
    }

    // ツール切り替えボタン
    if (uiManager.getButton(ButtonIndex::TOOL_BRUSH).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::BRUSH);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(ButtonIndex::TOOL_ERASER).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::ERASER);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(ButtonIndex::TOOL_LINE).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::LINE);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(ButtonIndex::TOOL_CIRCLE).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::CIRCLE);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(ButtonIndex::TOOL_ELLIPSE).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::ELLIPSE);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(ButtonIndex::TOOL_LARGE_TILE).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::LARGE_TILE);
        if (auto* largeTileTool = dynamic_cast<LargeTileTool*>(drawingManager.getCurrentTool())) {
            largeTileTool->setLargeTileId(currentLargeTileId);
        }
    }

    // ビュー操作
    if (uiManager.getButton(ButtonIndex::RESET_VIEW).isClicked(clickPos, true)) {
        canvasView.reset();
    }

    // ブラシサイズ
    if (uiManager.getButton(ButtonIndex::BRUSH_SMALL).isClicked(clickPos, true)) brushSize = 1;
    if (uiManager.getButton(ButtonIndex::BRUSH_MEDIUM).isClicked(clickPos, true)) brushSize = 3;
    if (uiManager.getButton(ButtonIndex::BRUSH_LARGE).isClicked(clickPos, true)) brushSize = 5;

    // 表示設定
    if (uiManager.getButton(ButtonIndex::TOGGLE_GRID).isClicked(clickPos, true)) {
        showGrid = !showGrid;
    }

    /*
    // パターン操作
    if (uiManager.getButton(ButtonIndex::ADD_PATTERN).isClicked(clickPos, true)) {
        tilePalette.addPattern(patternGrid.getTiles(), colorPanel.getColorSet());
    }
    */

    // パターン操作
    if (uiManager.getButton(ButtonIndex::ADD_PATTERN).isClicked(clickPos, true)) {
        // 新システム：現在のグローバルカラーインデックスでパターンを追加
        auto globalIndices = colorPanel.getGlobalColorIndices();
        tilePalette.addPatternWithGlobalColors(patternGrid.getTiles(), globalIndices);

        std::cout << "Added new pattern with global color indices: ["
            << globalIndices[0] << ", " << globalIndices[1] << ", " << globalIndices[2] << "]" << std::endl;
    }

    if (uiManager.getButton(ButtonIndex::SAVE_CHANGES).isClicked(clickPos, true) &&
        tilePalette.getSelectedIndex() >= 0) {
        tilePalette.updatePattern(tilePalette.getSelectedIndex(), patternGrid.getTiles());
        tilePalette.updateColorSet(tilePalette.getSelectedIndex(), colorPanel.getColorSet());
    }

    // ファイル操作
    handleFileOperations(clickPos, uiManager, tilePalette, patternGrid, colorPanel, canvas);

    // 通常のTilePalette処理（オーバーレイ非表示時のみ）
    if (!largeTilePaletteOverlay.getVisible()) {
        if (tilePalette.handleClick(clickPos)) {
            int selIdx = tilePalette.getSelectedIndex();
            /*
            if (selIdx >= 0) {
                colorPanel.setTarget(tilePalette.getSelectedColorSet());
                auto flat = tilePalette.getPattern(selIdx);
                std::vector<std::vector<int>> grid(3, std::vector<int>(3));
                for (int i = 0; i < 9; ++i) {
                    grid[i / 3][i % 3] = flat[i];
                }
                patternGrid.setTiles(grid);
            }
            */

            /*
            if (selIdx >= 0) {
                // 新システム：選択されたパターンのグローバルカラーインデックスをColorPanelに設定
                auto globalIndices = tilePalette.getGlobalColorIndices(selIdx);
                colorPanel.setGlobalColorIndices(globalIndices);

                // 後方互換性：従来の色セット設定も行う
                colorPanel.setTarget(tilePalette.getSelectedColorSet());
                auto flat = tilePalette.getPattern(selIdx);
                std::vector<std::vector<int>> grid(3, std::vector<int>(3));
                for (int i = 0; i < 9; ++i) {
                    grid[i / 3][i % 3] = flat[i];
                }
                patternGrid.setTiles(grid);

                std::cout << "Selected pattern [" << selIdx << "] with global color indices: ["
                    << globalIndices[0] << ", " << globalIndices[1] << ", " << globalIndices[2] << "]" << std::endl;
            }
            */
            if (selIdx >= 0) {
                // 新システム：選択されたパターンのグローバルカラーインデックスをColorPanelに設定
                auto globalIndices = tilePalette.getGlobalColorIndices(selIdx);
                colorPanel.setGlobalColorIndices(globalIndices);

                // パターンデータをPatternGridに設定
                auto flat = tilePalette.getPattern(selIdx);
                std::vector<std::vector<int>> grid(3, std::vector<int>(3));
                for (int i = 0; i < 9; ++i) {
                    grid[i / 3][i % 3] = flat[i];
                }
                patternGrid.setTiles(grid);

                // 後方互換性：従来の色セット設定も行う
                colorPanel.setTarget(tilePalette.getSelectedColorSet());

                std::cout << "Selected pattern [" << selIdx << "] with global color indices: ["
                    << globalIndices[0] << ", " << globalIndices[1] << ", " << globalIndices[2] << "]" << std::endl;
            }
        }
    }
}

/**
 * ファイル操作処理
 */
void handleFileOperations(const sf::Vector2i& clickPos, UIManager& uiManager,
    TilePalette& tilePalette, PatternGrid& patternGrid,
    ColorPanel& colorPanel, Canvas& canvas) {

    // プロジェクト保存
    if (uiManager.getButton(ButtonIndex::SAVE_FILE).isClicked(clickPos, true)) {
        const char* savePath = tinyfd_saveFileDialog("Save Project", "project.dat", 0, nullptr, nullptr);
        if (savePath) {
            saveProject(savePath, tilePalette.getAllPatterns(),
                tilePalette.getAllColorPalettes(), canvas.getTileIndices());
        }
    }

    // プロジェクト読み込み
    if (uiManager.getButton(ButtonIndex::LOAD_FILE).isClicked(clickPos, true)) {
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

    // PNG出力
    if (uiManager.getButton(ButtonIndex::EXPORT_PNG).isClicked(clickPos, true)) {
        exportImage(tilePalette, canvas, "png");
    }

    // JPG出力
    if (uiManager.getButton(ButtonIndex::EXPORT_JPG).isClicked(clickPos, true)) {
        exportImage(tilePalette, canvas, "jpg");
    }
}

/**
 * 画像出力処理
 */
void exportImage(TilePalette& tilePalette, Canvas& canvas, const std::string& format) {
    std::string defaultName = ImageExportHelper::generateDefaultFilename(format);
    std::string savePath = ImageExportHelper::showSaveDialog(
        "Export " + format + " Image", defaultName, format
    );

    if (!savePath.empty()) {
        // 拡張子チェック・追加
        std::string expectedExt = "." + format;
        if (format == "jpg") {
            if (savePath.substr(std::max(0, (int)savePath.length() - 4)) != ".jpg" &&
                savePath.substr(std::max(0, (int)savePath.length() - 5)) != ".jpeg") {
                savePath += ".jpg";
            }
        }
        else {
            if (savePath.substr(std::max(0, (int)savePath.length() - 4)) != expectedExt) {
                savePath += expectedExt;
            }
        }

        bool success = canvas.exportToImage(
            savePath,
            tilePalette.getAllPatterns(),
            tilePalette.getAllColorPalettes(),
            false, // グリッド線は出力しない
            0.0f, 1.0f
        );

        const char* message = success ?
            ("Image exported successfully:\n" + savePath).c_str() :
            "Failed to export image.";
        const char* type = success ? "info" : "error";

        tinyfd_messageBox(success ? "Export Complete" : "Export Failed", message, type, "ok", 1);
    }
}

/**
 * キーボード入力処理
 */
void handleKeyboardInput(const sf::Event& event, LargeTileManager& largeTileManager,
    int& currentLargeTileId, DrawingManager& drawingManager) {
    if (event.type != sf::Event::KeyPressed) return;

    // Rキーで回転
    if (event.key.code == sf::Keyboard::R) {
        largeTileManager.rotateCurrentTile();
        if (drawingManager.getCurrentToolType() == ToolManager::ToolType::LARGE_TILE) {
            if (auto* largeTileTool = dynamic_cast<LargeTileTool*>(drawingManager.getCurrentTool())) {
                largeTileTool->setLargeTileId(currentLargeTileId);
            }
        }
    }

    // 0-9キーで大型タイル選択
    if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9) {
        int tileId = event.key.code - sf::Keyboard::Num0;
        if (tileId < 12) {
            selectLargeTile(largeTileManager, drawingManager, currentLargeTileId, tileId);
        }
    }

    // Q, Wキーで大型タイル選択（10, 11）
    if (event.key.code == sf::Keyboard::Q) {
        selectLargeTile(largeTileManager, drawingManager, currentLargeTileId, 10);
    }
    if (event.key.code == sf::Keyboard::W) {
        selectLargeTile(largeTileManager, drawingManager, currentLargeTileId, 11);
    }
}

/**
 * 大型タイル選択のヘルパー
 */
void selectLargeTile(LargeTileManager& largeTileManager, DrawingManager& drawingManager,
    int& currentLargeTileId, int tileId) {
    currentLargeTileId = tileId;
    largeTileManager.selectLargeTile(currentLargeTileId);
    drawingManager.setTool(ToolManager::ToolType::LARGE_TILE);

    if (auto* largeTileTool = dynamic_cast<LargeTileTool*>(drawingManager.getCurrentTool())) {
        largeTileTool->setLargeTileId(currentLargeTileId);
    }
}

/**
 * ゲーム状態更新
 */
void updateGameState(const sf::Vector2i& mousePos, bool mousePressed, bool& isPanning,
    sf::Vector2i& lastPanPos, CanvasView& canvasView, DrawingManager& drawingManager,
    Canvas& canvas, TilePalette& tilePalette, ColorPanel& colorPanel,
    PatternGrid& patternGrid, UIManager& uiManager, float& gridSpacing,
    float& gridShrink, sf::Color& tileGridColor, int& selectedColorIndex,
    bool& patternChanged, int brushSize, GlobalColorPalette& globalColorPalette) {

    // パン操作更新
    if (isPanning && sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
        sf::Vector2f delta = static_cast<sf::Vector2f>(mousePos - lastPanPos);
        canvasView.pan(delta);
        lastPanPos = mousePos;
    }

    // 描画更新（ブラシサイズを正しく渡す）
    if (drawingManager.getIsDrawing()) {
        drawingManager.updateMovement(mousePos, canvas, canvasView,
            tilePalette.getSelectedIndex(), brushSize);
    }

    /*
    // カラーパネル更新
    bool colorSliderChanged = colorPanel.updateSliders(mousePos, mousePressed);
    if (colorSliderChanged && tilePalette.getSelectedIndex() >= 0) {
        tilePalette.updateColorSet(tilePalette.getSelectedIndex(), colorPanel.getColorSet());
        canvas.setDirty(true);

        // RGBスライダーで色が変更された場合、選択中のグローバルカラーに反映
        int currentColorIndex = colorPanel.getCurrentColorIndex();
        sf::Color newColor = colorPanel.getColorSet()[currentColorIndex];

        // 選択中のグローバルカラーパレットに新しい色を設定
        globalColorPalette.setColor(globalColorPalette.getSelectedIndex(), newColor);

        // ログ出力
        std::cout << "RGB slider changed - Updated Global Color ["
            << globalColorPalette.getSelectedIndex() << "] to RGB("
            << (int)newColor.r << ", " << (int)newColor.g << ", "
            << (int)newColor.b << ")" << std::endl;

    }
    */
    bool colorSliderChanged = colorPanel.updateSliders(mousePos, mousePressed);
    if (colorSliderChanged && tilePalette.getSelectedIndex() >= 0) {
        tilePalette.updateColorSet(tilePalette.getSelectedIndex(), colorPanel.getColorSet());
        canvas.setDirty(true);

        // RGBスライダーで色が変更された場合の処理
        // 新システム：現在編集中の色をグローバルカラーパレットに反映
        colorPanel.updateGlobalColorFromCurrent();

        // ログ出力（グローバルカラーインデックス情報を追加）
        int currentColorIndex = colorPanel.getCurrentColorIndex();
        int globalColorIndex = colorPanel.getCurrentSlotGlobalIndex();
        sf::Color newColor = colorPanel.getColorSet()[currentColorIndex];

        std::cout << "RGB slider changed - Updated Global Color ["
            << globalColorIndex << "] to RGB("
            << (int)newColor.r << ", " << (int)newColor.g << ", "
            << (int)newColor.b << ") via color slot [" << currentColorIndex << "]" << std::endl;
    }

    // マウス押下時の処理
    if (mousePressed) {
        colorPanel.handleEvent(mousePos, true, selectedColorIndex);

        if (patternGrid.handleClick(mousePos, selectedColorIndex)) {
            patternChanged = true;
        }
    }


    // スライダー更新
    sf::Color oldTileGridColor = tileGridColor;
    uiManager.updateSliders(mousePos, mousePressed, gridSpacing, gridShrink, tileGridColor);

    if (tileGridColor.r != oldTileGridColor.r ||
        tileGridColor.g != oldTileGridColor.g ||
        tileGridColor.b != oldTileGridColor.b) {
        canvas.setTileGridColor(tileGridColor);
    }

    // パターン変更処理
    if (patternChanged && tilePalette.getSelectedIndex() >= 0) {
        tilePalette.updatePattern(tilePalette.getSelectedIndex(), patternGrid.getTiles());
        canvas.setDirty(true);
        patternChanged = false;
    }

    // ボタン更新
    uiManager.updateButtons(mousePos);
}

/**
 * フレーム描画
 */

/*
void renderFrame(sf::RenderWindow& window, const sf::Font& font, PatternGrid& patternGrid,
    TilePalette& tilePalette, ColorPanel& colorPanel, Canvas& canvas,
    CanvasView& canvasView, LargeTilePaletteOverlay& largeTilePaletteOverlay,
    DrawingManager& drawingManager, UIManager& uiManager, const sf::Vector2i& mousePos,
    int selectedColorIndex, int brushSize, bool showGrid, float gridSpacing,
    float gridShrink, const sf::Color& tileGridColor, int currentLargeTileId,
    LargeTileManager& largeTileManager, GlobalColorPalette& globalColorPalette) {

    window.clear(sf::Color(30, 30, 30));

    // 情報表示
    renderInfoText(window, font, canvasView, drawingManager, currentLargeTileId,
        largeTileManager, brushSize, selectedColorIndex, tilePalette, canvas,globalColorPalette);

    // コンポーネント描画
    if (tilePalette.getSelectedIndex() >= 0) {
        colorPanel.setTarget(tilePalette.getSelectedColorSet());
    }

   // patternGrid.draw(window, colorPanel.getColorSet());
    // PatternGrid描画を修正：グローバルカラーを使用
    if (tilePalette.getSelectedIndex() >= 0) {
        auto globalIndices = colorPanel.getGlobalColorIndices();
        patternGrid.drawWithGlobalColors(window, globalColorPalette.getAllColors(), globalIndices);
    }
    else {
        // タイルが選択されていない場合はデフォルト
        patternGrid.drawWithGlobalColors(window, globalColorPalette.getAllColors(), { 0, 1, 2 });
    }


  //  tilePalette.draw(window, tilePalette.getAllColorPalettes());

    tilePalette.drawWithGlobalColors(window, globalColorPalette.getAllColors());

    colorPanel.draw(window);
    largeTilePaletteOverlay.draw(window);
    globalColorPalette.draw(window);

    // キャンバス描画
    canvas.drawWithView(window, canvasView,
        tilePalette.getAllPatterns(),
        tilePalette.getAllColorPalettes(),
        showGrid, gridSpacing, gridShrink);

    // UI描画
    uiManager.drawButtons(window, font, drawingManager.getCurrentToolType(), brushSize,
        largeTilePaletteOverlay.getVisible(), largeTileManager.getCurrentRotationDegrees());
    uiManager.drawSliders(window, gridSpacing, gridShrink, tileGridColor);

    // カーソル＆プレビュー描画
    if (canvas.containsInView(canvasView, mousePos)) {
        drawingManager.drawCursor(window, mousePos, canvasView, brushSize, canvas.getTileSize());
        drawingManager.drawPreview(window, mousePos, canvasView, brushSize);
    }

    window.display();
}
*/

void renderFrame(sf::RenderWindow& window, const sf::Font& font, PatternGrid& patternGrid,
    TilePalette& tilePalette, ColorPanel& colorPanel, Canvas& canvas,
    CanvasView& canvasView, LargeTilePaletteOverlay& largeTilePaletteOverlay,
    DrawingManager& drawingManager, UIManager& uiManager, const sf::Vector2i& mousePos,
    int selectedColorIndex, int brushSize, bool showGrid, float gridSpacing,
    float gridShrink, const sf::Color& tileGridColor, int currentLargeTileId,
    LargeTileManager& largeTileManager, GlobalColorPalette& globalColorPalette) {

    window.clear(sf::Color(30, 30, 30));

    // 情報表示
    renderInfoText(window, font, canvasView, drawingManager, currentLargeTileId,
        largeTileManager, brushSize, selectedColorIndex, tilePalette, canvas, globalColorPalette);

    // コンポーネント描画
    if (tilePalette.getSelectedIndex() >= 0) {
        colorPanel.setTarget(tilePalette.getSelectedColorSet());
    }

    // PatternGrid描画（グローバルカラー使用）
    if (tilePalette.getSelectedIndex() >= 0) {
        auto globalIndices = colorPanel.getGlobalColorIndices();
        patternGrid.drawWithGlobalColors(window, globalColorPalette.getAllColors(), globalIndices);
    }
    else {
        // タイルが選択されていない場合はデフォルト
        patternGrid.drawWithGlobalColors(window, globalColorPalette.getAllColors(), { 0, 1, 2 });
    }

    // TilePalette描画（グローバルカラー使用）
    tilePalette.drawWithGlobalColors(window, globalColorPalette.getAllColors());

    colorPanel.draw(window);
    largeTilePaletteOverlay.draw(window);
    globalColorPalette.draw(window);

    // キャンバス描画（グローバルカラー使用）
    // TilePaletteからグローバルカラーインデックス配列を取得
    std::vector<std::array<int, 3>> allGlobalColorIndices;
    int patternCount = tilePalette.getPatternCount();
    for (int i = 0; i < patternCount; ++i) {
        allGlobalColorIndices.push_back(tilePalette.getGlobalColorIndices(i));
    }

    // 新しいグローバルカラー対応メソッドを使用
    canvas.drawWithViewAndGlobalColors(window, canvasView,
        tilePalette.getAllPatterns(),
        allGlobalColorIndices,
        globalColorPalette.getAllColors(),
        showGrid, gridSpacing, gridShrink);

    // UI描画
    uiManager.drawButtons(window, font, drawingManager.getCurrentToolType(), brushSize,
        largeTilePaletteOverlay.getVisible(), largeTileManager.getCurrentRotationDegrees());
    uiManager.drawSliders(window, gridSpacing, gridShrink, tileGridColor);

    // カーソル＆プレビュー描画
    if (canvas.containsInView(canvasView, mousePos)) {
        drawingManager.drawCursor(window, mousePos, canvasView, brushSize, canvas.getTileSize());
        drawingManager.drawPreview(window, mousePos, canvasView, brushSize);
    }

    window.display();
}


/**
 * 情報テキスト描画
 */
void renderInfoText(sf::RenderWindow& window, const sf::Font& font, CanvasView& canvasView,
    DrawingManager& drawingManager, int currentLargeTileId,
    LargeTileManager& largeTileManager, int brushSize,
    int selectedColorIndex, TilePalette& tilePalette, Canvas& canvas,
    GlobalColorPalette& globalColorPalette) {

    // ズーム情報
    drawText(window, font, canvasView.getStatusString(),
        14, sf::Vector2f(20, 20), sf::Color::Cyan);

    // ツール情報
    std::string toolInfo = "Tool: " + drawingManager.getCurrentToolName();
    if (drawingManager.getCurrentToolType() == ToolManager::ToolType::LARGE_TILE) {
        int rotationDegrees = largeTileManager.getCurrentRotationDegrees();
        toolInfo += " [" + std::to_string(currentLargeTileId) + "]";
        if (rotationDegrees != 0) {
            toolInfo += " " + std::to_string(rotationDegrees) + "°";
        }
    }
    else {
        toolInfo += " | Brush Size: " + std::to_string(brushSize);
    }
    drawText(window, font, toolInfo, 14, sf::Vector2f(20, 40), sf::Color::Yellow);

    // 操作説明
    drawText(window, font, "Mouse Wheel: Zoom | Middle Drag: Pan | Left Click: Draw",
        12, sf::Vector2f(20, 60), sf::Color(200, 200, 200));

    // ツール別説明
    renderToolSpecificInfo(window, font, drawingManager, largeTileManager, currentLargeTileId);

    // ステータス情報
    drawText(window, font, "Selected Color: " + std::to_string(selectedColorIndex),
        14, sf::Vector2f(20, 840), sf::Color::White);

    if (tilePalette.getSelectedIndex() >= 0) {
        drawText(window, font, "Selected Tile: " + std::to_string(tilePalette.getSelectedIndex()),
            14, sf::Vector2f(20, 860), sf::Color::White);
    }
    else {
        drawText(window, font, "No Tile Selected", 14, sf::Vector2f(20, 860), sf::Color::White);
    }

    // キャンバス情報
    auto canvasSize = canvas.getCanvasPixelSize();
    std::string canvasInfo = "Canvas: " + std::to_string(canvasSize.first) + "x" +
        std::to_string(canvasSize.second) + " px";
    drawText(window, font, canvasInfo, 14, sf::Vector2f(20, 820), sf::Color(200, 200, 200));

    // デバッグ情報（F1キー押下時）
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
        auto stats = drawingManager.getDrawingStats();
        std::string debugInfo = "Drawing: " + std::string(stats.isDrawing ? "YES" : "NO") +
            " | Moved: " + std::string(stats.hasMoved ? "YES" : "NO");
        drawText(window, font, debugInfo, 12, sf::Vector2f(20, 100), sf::Color::Green);
    }


    // GlobalColorPalette情報を追加
    int globalColorIndex = globalColorPalette.getSelectedIndex();
    sf::Color globalColor = globalColorPalette.getSelectedColor();
    std::string globalColorInfo = "Global Color: [" + std::to_string(globalColorIndex) + "] RGB(" +
        std::to_string((int)globalColor.r) + ", " + std::to_string((int)globalColor.g) + ", " +
        std::to_string((int)globalColor.b) + ")";
    drawText(window, font, globalColorInfo, 14, sf::Vector2f(20, 800), sf::Color(100, 255, 100));

}

/**
 * ツール固有の情報表示
 */
void renderToolSpecificInfo(sf::RenderWindow& window, const sf::Font& font,
    DrawingManager& drawingManager, LargeTileManager& largeTileManager,
    int currentLargeTileId) {
    auto toolType = drawingManager.getCurrentToolType();

    if (toolType == ToolManager::ToolType::LARGE_TILE) {
        std::string largeTileInfo = "Large Tile: 0-9,Q,W keys to select | Current: " +
            std::to_string(currentLargeTileId);
        drawText(window, font, largeTileInfo, 12, sf::Vector2f(20, 80), sf::Color(255, 200, 100));
        drawRotationGuide(window, font, largeTileManager.getCurrentRotationDegrees());
    }
    else if (toolType == ToolManager::ToolType::LINE) {
        drawText(window, font, "Line Tool: Click and drag to draw lines",
            12, sf::Vector2f(20, 80), sf::Color(255, 200, 100));
    }
    else if (toolType == ToolManager::ToolType::CIRCLE) {
        drawText(window, font, "Circle Tool: Click center and drag to set radius",
            12, sf::Vector2f(20, 80), sf::Color(255, 200, 100));
    }
    else if (toolType == ToolManager::ToolType::ELLIPSE) {
        drawText(window, font, "Ellipse Tool: Click and drag diagonal to draw ellipse",
            12, sf::Vector2f(20, 80), sf::Color(255, 200, 100));
    }
}