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
#include "tinyfiledialogs.h"
#include <iostream>
#include <set>
#include <vector>
#include <memory>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

extern LargeTileManager largeTileManager;

const int WINDOW_WIDTH = 1600;
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
 * UIManagerクラス - ボタンとスライダーの管理
 */
class UIManager {
private:
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<sf::RectangleShape> sliders;
    std::vector<sf::CircleShape> knobs;
    std::vector<sf::Text> labels;

public:
    // ボタンインデックス定義
    enum ButtonIndex {
        ADD_PATTERN = 0, SAVE_CHANGES = 1, SAVE_FILE = 2, LOAD_FILE = 3,
        EXPORT_PNG = 4, EXPORT_JPG = 5, TOGGLE_GRID = 6,
        BRUSH_SMALL = 7, BRUSH_MEDIUM = 8, BRUSH_LARGE = 9,
        TOOL_BRUSH = 10, TOOL_ERASER = 11, TOOL_LINE = 12,
        TOOL_CIRCLE = 13, TOOL_ELLIPSE = 14, TOOL_LARGE_TILE = 15,
        LARGE_TILE_PALETTE_TOGGLE = 16, ROTATE_BUTTON = 17, RESET_VIEW = 18
    };

    UIManager(const sf::Font& font) {
        initializeButtons();
        initializeSliders(font);
    }

    void initializeButtons() {
        // パターン操作
        buttons.emplace_back(std::make_unique<Button>("Add to Palette", sf::Vector2f(20, 450), sf::Vector2f(100, 25)));
        buttons.emplace_back(std::make_unique<Button>("Save Changes", sf::Vector2f(150, 450), sf::Vector2f(100, 25)));

        // ファイル操作
        buttons.emplace_back(std::make_unique<Button>("Save to File", sf::Vector2f(20, 600), sf::Vector2f(100, 30)));
        buttons.emplace_back(std::make_unique<Button>("Load from File", sf::Vector2f(130, 600), sf::Vector2f(100, 30)));
        buttons.emplace_back(std::make_unique<Button>("Export PNG", sf::Vector2f(240, 600), sf::Vector2f(100, 30)));
        buttons.emplace_back(std::make_unique<Button>("Export JPG", sf::Vector2f(200, 850), sf::Vector2f(100, 30)));

        // 表示設定
        buttons.emplace_back(std::make_unique<Button>("Toggle Grid", sf::Vector2f(20, 640), sf::Vector2f(120, 30)));

        // ブラシサイズ
        buttons.emplace_back(std::make_unique<Button>("1x1", sf::Vector2f(20, 690), sf::Vector2f(50, 30)));
        buttons.emplace_back(std::make_unique<Button>("3x3", sf::Vector2f(80, 690), sf::Vector2f(50, 30)));
        buttons.emplace_back(std::make_unique<Button>("5x5", sf::Vector2f(140, 690), sf::Vector2f(50, 30)));

        // ツール選択
        buttons.emplace_back(std::make_unique<Button>("Brush", sf::Vector2f(20, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Eraser", sf::Vector2f(80, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Line", sf::Vector2f(140, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Circle", sf::Vector2f(200, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Ellipse", sf::Vector2f(260, 730), sf::Vector2f(60, 30)));
        buttons.emplace_back(std::make_unique<Button>("Large", sf::Vector2f(200, 690), sf::Vector2f(60, 30)));

        // 大型タイル関連
        buttons.emplace_back(std::make_unique<Button>("L-Palette", sf::Vector2f(250, 690), sf::Vector2f(80, 30)));
        buttons.emplace_back(std::make_unique<Button>("Rotate", sf::Vector2f(330, 690), sf::Vector2f(70, 30)));

        // ビュー操作
        buttons.emplace_back(std::make_unique<Button>("Reset View", sf::Vector2f(20, 770), sf::Vector2f(100, 30)));
    }

    void initializeSliders(const sf::Font& font) {
        // グリッド調整スライダー
        sliders.emplace_back(sf::Vector2f(120, 10));
        sliders.emplace_back(sf::Vector2f(120, 10));
        sliders[0].setPosition(70, 550);
        sliders[1].setPosition(70, 580);
        sliders[0].setFillColor(sf::Color(80, 80, 80));
        sliders[1].setFillColor(sf::Color(80, 80, 80));

        // タイル内部グリッド色スライダー（RGB）
        for (int i = 0; i < 3; ++i) {
            sliders.emplace_back(sf::Vector2f(100, 8));
            sliders[2 + i].setPosition(240, 550 + i * 20);
        }
        sliders[2].setFillColor(sf::Color(100, 50, 50));
        sliders[3].setFillColor(sf::Color(50, 100, 50));
        sliders[4].setFillColor(sf::Color(50, 50, 100));

        // ノブ初期化
        for (int i = 0; i < 5; ++i) {
            knobs.emplace_back(i < 2 ? 5 : 4);
            knobs[i].setFillColor(i < 2 ? sf::Color::White :
                (i == 2 ? sf::Color::Red : (i == 3 ? sf::Color::Green : sf::Color::Blue)));
        }

        // ラベル初期化
        labels.emplace_back("Spacing", font, 14);
        labels.emplace_back("Shrink", font, 14);
        labels.emplace_back("Tile Grid Color", font, 12);
        labels.emplace_back("R", font, 11);
        labels.emplace_back("G", font, 11);
        labels.emplace_back("B", font, 11);

        labels[0].setPosition(20, 545);
        labels[1].setPosition(20, 575);
        labels[2].setPosition(220, 530);
        labels[3].setPosition(220, 547);
        labels[4].setPosition(220, 567);
        labels[5].setPosition(220, 587);
    }

    Button& getButton(ButtonIndex index) {
        return *buttons[index];
    }

    void updateButtons(const sf::Vector2i& mousePos) {
        for (auto& button : buttons) {
            button->update(mousePos);
        }
    }

    void drawButtons(sf::RenderWindow& window, const sf::Font& font, ToolManager::ToolType activeToolType,
        int currentBrushSize, bool largeTilePaletteVisible = false, int rotationDegrees = 0) {
        for (size_t i = 0; i < buttons.size(); ++i) {
            bool isActive = isButtonActive(i, activeToolType, currentBrushSize, largeTilePaletteVisible, rotationDegrees);

            if (isActive) {
                buttons[i]->setActiveState(true);
            }

            buttons[i]->draw(window, font);

            if (i == ROTATE_BUTTON) {
                drawRotationIndicator(window, font, buttons[i]->getPosition(), rotationDegrees);
            }

            if (isActive) {
                buttons[i]->setActiveState(false);
            }
        }
    }

    void updateSliders(const sf::Vector2i& mousePos, bool mousePressed,
        float& gridSpacing, float& gridShrink, sf::Color& tileGridColor) {
        // グリッドスライダー
        updateSlider(0, mousePos, mousePressed, gridSpacing);
        updateSlider(1, mousePos, mousePressed, gridShrink);

        // RGBスライダー
        updateColorSlider(2, mousePos, mousePressed, tileGridColor.r);
        updateColorSlider(3, mousePos, mousePressed, tileGridColor.g);
        updateColorSlider(4, mousePos, mousePressed, tileGridColor.b);
    }

    void drawSliders(sf::RenderWindow& window, float gridSpacing, float gridShrink, const sf::Color& tileGridColor) {
        // ラベル描画
        for (auto& label : labels) {
            window.draw(label);
        }

        // スライダー描画
        for (auto& slider : sliders) {
            window.draw(slider);
        }

        // ノブ位置更新・描画
        updateKnobPositions(gridSpacing, gridShrink, tileGridColor);
        for (auto& knob : knobs) {
            window.draw(knob);
        }

        // 色プレビュー
        drawColorPreview(window, tileGridColor);
    }

private:
    bool isButtonActive(size_t i, ToolManager::ToolType activeToolType, int currentBrushSize,
        bool largeTilePaletteVisible, int rotationDegrees) {
        // ツールボタンのアクティブ状態
        if (i == TOOL_BRUSH && activeToolType == ToolManager::ToolType::BRUSH) return true;
        if (i == TOOL_ERASER && activeToolType == ToolManager::ToolType::ERASER) return true;
        if (i == TOOL_LINE && activeToolType == ToolManager::ToolType::LINE) return true;
        if (i == TOOL_CIRCLE && activeToolType == ToolManager::ToolType::CIRCLE) return true;
        if (i == TOOL_ELLIPSE && activeToolType == ToolManager::ToolType::ELLIPSE) return true;
        if (i == TOOL_LARGE_TILE && activeToolType == ToolManager::ToolType::LARGE_TILE) return true;

        // ブラシサイズボタンのアクティブ状態
        if (i == BRUSH_SMALL && currentBrushSize == 1) return true;
        if (i == BRUSH_MEDIUM && currentBrushSize == 3) return true;
        if (i == BRUSH_LARGE && currentBrushSize == 5) return true;

        // その他のアクティブ状態
        if (i == LARGE_TILE_PALETTE_TOGGLE && largeTilePaletteVisible) return true;
        if (i == ROTATE_BUTTON && rotationDegrees != 0) return true;

        return false;
    }

    void updateSlider(int index, const sf::Vector2i& mousePos, bool mousePressed, float& value) {
        sf::FloatRect bounds = sliders[index].getGlobalBounds();
        if (mousePressed && bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            float ratio = (mousePos.x - bounds.left) / bounds.width;
            value = std::clamp(ratio, 0.0f, 1.0f);
        }
    }

    void updateColorSlider(int index, const sf::Vector2i& mousePos, bool mousePressed, sf::Uint8& colorComponent) {
        sf::FloatRect bounds = sliders[index].getGlobalBounds();
        if (mousePressed && bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            float ratio = (mousePos.x - bounds.left) / bounds.width;
            colorComponent = static_cast<sf::Uint8>(ratio * 255);
        }
    }

    void updateKnobPositions(float gridSpacing, float gridShrink, const sf::Color& tileGridColor) {
        // グリッドノブ
        knobs[0].setPosition(sliders[0].getPosition().x + sliders[0].getSize().x * gridSpacing - 5,
            sliders[0].getPosition().y - 3);
        knobs[1].setPosition(sliders[1].getPosition().x + sliders[1].getSize().x * gridShrink - 5,
            sliders[1].getPosition().y - 3);

        // RGBノブ
        float ratios[] = { tileGridColor.r / 255.0f, tileGridColor.g / 255.0f, tileGridColor.b / 255.0f };
        for (int i = 0; i < 3; ++i) {
            knobs[2 + i].setPosition(sliders[2 + i].getPosition().x + sliders[2 + i].getSize().x * ratios[i] - 4,
                sliders[2 + i].getPosition().y - 2);
        }
    }

    void drawColorPreview(sf::RenderWindow& window, const sf::Color& tileGridColor) {
        sf::RectangleShape colorPreview(sf::Vector2f(30, 10));
        colorPreview.setPosition(310, 530);
        colorPreview.setFillColor(tileGridColor);
        colorPreview.setOutlineThickness(1);
        colorPreview.setOutlineColor(sf::Color::White);
        window.draw(colorPreview);
    }

    void drawRotationIndicator(sf::RenderWindow& window, const sf::Font& font,
        const sf::Vector2f& buttonPos, int rotationDegrees) {
        if (rotationDegrees == 0) return;

        sf::Text rotationText(std::to_string(rotationDegrees) + "°", font, 10);
        rotationText.setPosition(buttonPos.x + 75, buttonPos.y + 20);
        rotationText.setFillColor(sf::Color::Yellow);
        window.draw(rotationText);

        drawRotationArrow(window, buttonPos, rotationDegrees);
    }

    void drawRotationArrow(sf::RenderWindow& window, const sf::Vector2f& buttonPos, int rotationDegrees) {
        sf::Vector2f center(buttonPos.x + 35, buttonPos.y + 15);
        float radius = 8.0f;
        float angleRad = rotationDegrees * M_PI / 180.0f;

        sf::Vector2f start(center.x + radius * cos(angleRad), center.y + radius * sin(angleRad));
        sf::Vector2f end(center.x + (radius - 3) * cos(angleRad + M_PI), center.y + (radius - 3) * sin(angleRad + M_PI));

        sf::VertexArray arrow(sf::Lines);
        arrow.append(sf::Vertex(start, sf::Color::Cyan));
        arrow.append(sf::Vertex(end, sf::Color::Cyan));
        window.draw(arrow);

        sf::CircleShape arrowHead(2.0f);
        arrowHead.setOrigin(2.0f, 2.0f);
        arrowHead.setPosition(start);
        arrowHead.setFillColor(sf::Color::Cyan);
        window.draw(arrowHead);
    }
};

// ===== 関数宣言 =====
void handleButtonClicks(const sf::Vector2i& clickPos, UIManager& uiManager,
    DrawingManager& drawingManager, LargeTilePaletteOverlay& largeTilePaletteOverlay,
    LargeTileManager& largeTileManager, int& currentLargeTileId,
    int& brushSize, bool& showGrid, TilePalette& tilePalette,
    PatternGrid& patternGrid, ColorPanel& colorPanel,
    Canvas& canvas, CanvasView& canvasView);

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
    bool& patternChanged, int brushSize);

void renderFrame(sf::RenderWindow& window, const sf::Font& font, PatternGrid& patternGrid,
    TilePalette& tilePalette, ColorPanel& colorPanel, Canvas& canvas,
    CanvasView& canvasView, LargeTilePaletteOverlay& largeTilePaletteOverlay,
    DrawingManager& drawingManager, UIManager& uiManager, const sf::Vector2i& mousePos,
    int selectedColorIndex, int brushSize, bool showGrid, float gridSpacing,
    float gridShrink, const sf::Color& tileGridColor, int currentLargeTileId,
    LargeTileManager& largeTileManager);

void renderInfoText(sf::RenderWindow& window, const sf::Font& font, CanvasView& canvasView,
    DrawingManager& drawingManager, int currentLargeTileId,
    LargeTileManager& largeTileManager, int brushSize,
    int selectedColorIndex, TilePalette& tilePalette, Canvas& canvas);

void renderToolSpecificInfo(sf::RenderWindow& window, const sf::Font& font,
    DrawingManager& drawingManager, LargeTileManager& largeTileManager,
    int currentLargeTileId);

// ユーティリティ関数
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
    Canvas canvas(160, 160, 3, sf::Vector2f(360, 20));
    CanvasView canvasView(sf::Vector2f(360, 20), window.getSize());
    DrawingManager drawingManager;
    UIManager uiManager(font);
    LargeTilePaletteOverlay largeTilePaletteOverlay(sf::Vector2f(1350, 20), 50);

    // 位置設定
    patternGrid.setPosition(sf::Vector2f(20, 150));
    colorPanel.setPosition(sf::Vector2f(20, 310));
    tilePalette.setPosition(sf::Vector2f(1350, 20));

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
                    tilePalette, patternGrid, colorPanel, canvas, canvasView);

                // 描画開始
                if (canvas.containsInView(canvasView, clickPos)) {
                    drawingManager.startDrawing(clickPos, canvas, canvasView,
                        tilePalette.getSelectedIndex(), brushSize);
                }

                // パン開始
                if (event.mouseButton.button == sf::Mouse::Middle) {
                    isPanning = true;
                    lastPanPos = mousePos;
                }
            }

            // マウスリリース処理
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Middle) {
                    isPanning = false;
                }

                if (event.mouseButton.button == sf::Mouse::Left && drawingManager.getIsDrawing()) {
                    sf::Vector2i releasePos(event.mouseButton.x, event.mouseButton.y);
                    drawingManager.stopDrawing(releasePos, canvas, canvasView,
                        tilePalette.getSelectedIndex(), brushSize);
                }
            }

            // キーボードショートカット
            handleKeyboardInput(event, largeTileManager, currentLargeTileId, drawingManager);
        }

        // 更新処理
        updateGameState(mousePos, mousePressed, isPanning, lastPanPos, canvasView,
            drawingManager, canvas, tilePalette, colorPanel, patternGrid,
            uiManager, gridSpacing, gridShrink, tileGridColor,
            selectedColorIndex, patternChanged, brushSize);

        // 描画処理
        renderFrame(window, font, patternGrid, tilePalette, colorPanel, canvas, canvasView,
            largeTilePaletteOverlay, drawingManager, uiManager, mousePos,
            selectedColorIndex, brushSize, showGrid, gridSpacing, gridShrink,
            tileGridColor, currentLargeTileId, largeTileManager);
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
    Canvas& canvas, CanvasView& canvasView) {

    // 回転ボタン
    if (uiManager.getButton(UIManager::ROTATE_BUTTON).isClicked(clickPos, true)) {
        largeTileManager.rotateCurrentTile();
    }

    // 大型タイルパレット表示切り替え
    if (uiManager.getButton(UIManager::LARGE_TILE_PALETTE_TOGGLE).isClicked(clickPos, true)) {
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
    if (uiManager.getButton(UIManager::TOOL_BRUSH).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::BRUSH);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(UIManager::TOOL_ERASER).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::ERASER);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(UIManager::TOOL_LINE).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::LINE);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(UIManager::TOOL_CIRCLE).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::CIRCLE);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(UIManager::TOOL_ELLIPSE).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::ELLIPSE);
        largeTilePaletteOverlay.setVisible(false);
    }
    if (uiManager.getButton(UIManager::TOOL_LARGE_TILE).isClicked(clickPos, true)) {
        drawingManager.setTool(ToolManager::ToolType::LARGE_TILE);
        if (auto* largeTileTool = dynamic_cast<LargeTileTool*>(drawingManager.getCurrentTool())) {
            largeTileTool->setLargeTileId(currentLargeTileId);
        }
    }

    // ビュー操作
    if (uiManager.getButton(UIManager::RESET_VIEW).isClicked(clickPos, true)) {
        canvasView.reset();
    }

    // ブラシサイズ
    if (uiManager.getButton(UIManager::BRUSH_SMALL).isClicked(clickPos, true)) brushSize = 1;
    if (uiManager.getButton(UIManager::BRUSH_MEDIUM).isClicked(clickPos, true)) brushSize = 3;
    if (uiManager.getButton(UIManager::BRUSH_LARGE).isClicked(clickPos, true)) brushSize = 5;

    // 表示設定
    if (uiManager.getButton(UIManager::TOGGLE_GRID).isClicked(clickPos, true)) {
        showGrid = !showGrid;
    }

    // パターン操作
    if (uiManager.getButton(UIManager::ADD_PATTERN).isClicked(clickPos, true)) {
        tilePalette.addPattern(patternGrid.getTiles(), colorPanel.getColorSet());
    }

    if (uiManager.getButton(UIManager::SAVE_CHANGES).isClicked(clickPos, true) &&
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
}

/**
 * ファイル操作処理
 */
void handleFileOperations(const sf::Vector2i& clickPos, UIManager& uiManager,
    TilePalette& tilePalette, PatternGrid& patternGrid,
    ColorPanel& colorPanel, Canvas& canvas) {

    // プロジェクト保存
    if (uiManager.getButton(UIManager::SAVE_FILE).isClicked(clickPos, true)) {
        const char* savePath = tinyfd_saveFileDialog("Save Project", "project.dat", 0, nullptr, nullptr);
        if (savePath) {
            saveProject(savePath, tilePalette.getAllPatterns(),
                tilePalette.getAllColorPalettes(), canvas.getTileIndices());
        }
    }

    // プロジェクト読み込み
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

    // PNG出力
    if (uiManager.getButton(UIManager::EXPORT_PNG).isClicked(clickPos, true)) {
        exportImage(tilePalette, canvas, "png");
    }

    // JPG出力
    if (uiManager.getButton(UIManager::EXPORT_JPG).isClicked(clickPos, true)) {
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

        // 注意：gridSpacingとgridShrinkの値を適切に渡す必要がある
        // 現在はデフォルト値を使用しているが、実際の値を渡すべき
        bool success = canvas.exportToImage(
            savePath,
            tilePalette.getAllPatterns(),
            tilePalette.getAllColorPalettes(),
            // format == "png", // PNGのみグリッド表示
            false,
            0.0f, 1.0f // TODO: 実際のgridSpacing, gridShrink値を渡す
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
    bool& patternChanged, int brushSize) {

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

    // カラーパネル更新
    bool colorSliderChanged = colorPanel.updateSliders(mousePos, mousePressed);
    if (colorSliderChanged && tilePalette.getSelectedIndex() >= 0) {
        tilePalette.updateColorSet(tilePalette.getSelectedIndex(), colorPanel.getColorSet());
        canvas.setDirty(true);
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
void renderFrame(sf::RenderWindow& window, const sf::Font& font, PatternGrid& patternGrid,
    TilePalette& tilePalette, ColorPanel& colorPanel, Canvas& canvas,
    CanvasView& canvasView, LargeTilePaletteOverlay& largeTilePaletteOverlay,
    DrawingManager& drawingManager, UIManager& uiManager, const sf::Vector2i& mousePos,
    int selectedColorIndex, int brushSize, bool showGrid, float gridSpacing,
    float gridShrink, const sf::Color& tileGridColor, int currentLargeTileId,
    LargeTileManager& largeTileManager) {

    window.clear(sf::Color(30, 30, 30));

    // 情報表示
    renderInfoText(window, font, canvasView, drawingManager, currentLargeTileId,
        largeTileManager, brushSize, selectedColorIndex, tilePalette, canvas);

    // コンポーネント描画
    if (tilePalette.getSelectedIndex() >= 0) {
        colorPanel.setTarget(tilePalette.getSelectedColorSet());
    }

    patternGrid.draw(window, colorPanel.getColorSet());
    tilePalette.draw(window, tilePalette.getAllColorPalettes());
    colorPanel.draw(window);
    largeTilePaletteOverlay.draw(window);

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

/**
 * 情報テキスト描画
 */
void renderInfoText(sf::RenderWindow& window, const sf::Font& font, CanvasView& canvasView,
    DrawingManager& drawingManager, int currentLargeTileId,
    LargeTileManager& largeTileManager, int brushSize,
    int selectedColorIndex, TilePalette& tilePalette, Canvas& canvas) {

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