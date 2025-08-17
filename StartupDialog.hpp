#pragma once
// ===== StartupDialog.hpp =====

#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct CanvasSettings {
    int tileSize = 3;
    int canvasWidth = 128;
    int canvasHeight = 128;
};

class StartupDialog {
private:
    sf::Font& font;
    sf::RenderWindow& window;
    CanvasSettings settings;

    // UI要素
    sf::RectangleShape dialogBox;
    sf::RectangleShape okButton;
    sf::RectangleShape cancelButton;

    // 入力フィールド
    sf::RectangleShape tileSizeField;
    sf::RectangleShape widthField;
    sf::RectangleShape heightField;

    // 入力状態
    enum class ActiveField { NONE, TILE_SIZE, WIDTH, HEIGHT };
    ActiveField activeField = ActiveField::NONE;
    
    std::string tileSizeInput = "3";
    std::string widthInput = "128";
    std::string heightInput = "128";

    // ヘルパーメソッド
    void drawText(const std::string& text, float x, float y, int size, sf::Color color);
    void drawInputField(sf::RenderWindow& window, const sf::RectangleShape& field,
        const std::string& value, bool isActive);
    bool isValidInput(const std::string& input, int min, int max);
    void handleTextInput(sf::Uint32 unicode);
    void validateAndUpdateSettings();

public:
    StartupDialog(sf::RenderWindow& window, sf::Font& font);

    // ダイアログを表示し、設定を取得
    bool showDialog(CanvasSettings& outSettings);

    // イベント処理
    bool handleEvent(const sf::Event& event);
    void draw();
};