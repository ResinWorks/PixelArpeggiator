#define _USE_MATH_DEFINES

#include "UIManager.hpp"

UIManager::UIManager(const sf::Font& font) {
    initializeButtons();
    initializeSliders(font);
}

void UIManager::initializeButtons() {
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

void UIManager::initializeSliders(const sf::Font& font) {
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

Button& UIManager::getButton(ButtonIndex index) {
    return *buttons[static_cast<size_t>(index)];
}

void UIManager::updateButtons(const sf::Vector2i& mousePos) {
    for (auto& button : buttons) {
        button->update(mousePos);
    }
}

void UIManager::drawButtons(sf::RenderWindow& window, const sf::Font& font, ToolManager::ToolType activeToolType,
    int currentBrushSize, bool largeTilePaletteVisible, int rotationDegrees) {
    for (size_t i = 0; i < buttons.size(); ++i) {
        bool isActive = isButtonActive(i, activeToolType, currentBrushSize, largeTilePaletteVisible, rotationDegrees);

        if (isActive) {
            buttons[i]->setActiveState(true);
        }

        buttons[i]->draw(window, font);

        if (i == static_cast<size_t>(ButtonIndex::ROTATE_BUTTON)) {
            drawRotationIndicator(window, font, buttons[i]->getPosition(), rotationDegrees);
        }

        if (isActive) {
            buttons[i]->setActiveState(false);
        }
    }
}

void UIManager::updateSliders(const sf::Vector2i& mousePos, bool mousePressed,
    float& gridSpacing, float& gridShrink, sf::Color& tileGridColor) {
    // グリッドスライダー
    updateSlider(0, mousePos, mousePressed, gridSpacing);
    updateSlider(1, mousePos, mousePressed, gridShrink);

    // RGBスライダー
    updateColorSlider(2, mousePos, mousePressed, tileGridColor.r);
    updateColorSlider(3, mousePos, mousePressed, tileGridColor.g);
    updateColorSlider(4, mousePos, mousePressed, tileGridColor.b);
}

void UIManager::drawSliders(sf::RenderWindow& window, float gridSpacing, float gridShrink, const sf::Color& tileGridColor) {
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

bool UIManager::isButtonActive(size_t i, ToolManager::ToolType activeToolType, int currentBrushSize,
    bool largeTilePaletteVisible, int rotationDegrees) {
    // ツールボタンのアクティブ状態
    if (i == static_cast<size_t>(ButtonIndex::TOOL_BRUSH) && activeToolType == ToolManager::ToolType::BRUSH) return true;
    if (i == static_cast<size_t>(ButtonIndex::TOOL_ERASER) && activeToolType == ToolManager::ToolType::ERASER) return true;
    if (i == static_cast<size_t>(ButtonIndex::TOOL_LINE) && activeToolType == ToolManager::ToolType::LINE) return true;
    if (i == static_cast<size_t>(ButtonIndex::TOOL_CIRCLE) && activeToolType == ToolManager::ToolType::CIRCLE) return true;
    if (i == static_cast<size_t>(ButtonIndex::TOOL_ELLIPSE) && activeToolType == ToolManager::ToolType::ELLIPSE) return true;
    if (i == static_cast<size_t>(ButtonIndex::TOOL_LARGE_TILE) && activeToolType == ToolManager::ToolType::LARGE_TILE) return true;

    // ブラシサイズボタンのアクティブ状態
    if (i == static_cast<size_t>(ButtonIndex::BRUSH_SMALL) && currentBrushSize == 1) return true;
    if (i == static_cast<size_t>(ButtonIndex::BRUSH_MEDIUM) && currentBrushSize == 3) return true;
    if (i == static_cast<size_t>(ButtonIndex::BRUSH_LARGE) && currentBrushSize == 5) return true;

    // その他のアクティブ状態
    if (i == static_cast<size_t>(ButtonIndex::LARGE_TILE_PALETTE_TOGGLE) && largeTilePaletteVisible) return true;
    if (i == static_cast<size_t>(ButtonIndex::ROTATE_BUTTON) && rotationDegrees != 0) return true;

    return false;
}

void UIManager::updateSlider(int index, const sf::Vector2i& mousePos, bool mousePressed, float& value) {
    sf::FloatRect bounds = sliders[index].getGlobalBounds();
    if (mousePressed && bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
        float ratio = (mousePos.x - bounds.left) / bounds.width;
        value = std::clamp(ratio, 0.0f, 1.0f);
    }
}

void UIManager::updateColorSlider(int index, const sf::Vector2i& mousePos, bool mousePressed, sf::Uint8& colorComponent) {
    sf::FloatRect bounds = sliders[index].getGlobalBounds();
    if (mousePressed && bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
        float ratio = (mousePos.x - bounds.left) / bounds.width;
        colorComponent = static_cast<sf::Uint8>(ratio * 255);
    }
}

void UIManager::updateKnobPositions(float gridSpacing, float gridShrink, const sf::Color& tileGridColor) {
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

void UIManager::drawColorPreview(sf::RenderWindow& window, const sf::Color& tileGridColor) {
    sf::RectangleShape colorPreview(sf::Vector2f(30, 10));
    colorPreview.setPosition(310, 530);
    colorPreview.setFillColor(tileGridColor);
    colorPreview.setOutlineThickness(1);
    colorPreview.setOutlineColor(sf::Color::White);
    window.draw(colorPreview);
}

void UIManager::drawRotationIndicator(sf::RenderWindow& window, const sf::Font& font,
    const sf::Vector2f& buttonPos, int rotationDegrees) {
    if (rotationDegrees == 0) return;

    sf::Text rotationText(std::to_string(rotationDegrees) + "°", font, 10);
    rotationText.setPosition(buttonPos.x + 75, buttonPos.y + 20);
    rotationText.setFillColor(sf::Color::Yellow);
    window.draw(rotationText);

    drawRotationArrow(window, buttonPos, rotationDegrees);
}

void UIManager::drawRotationArrow(sf::RenderWindow& window, const sf::Vector2f& buttonPos, int rotationDegrees) {
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