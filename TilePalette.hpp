//===== TilePalette.hpp (�C����) =====
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

class TilePalette {
private:
    std::vector<std::vector<int>> patterns; // �e�^�C���̃p�^�[���i3x3�̐F�C���f�b�N�X�j
    sf::Vector2f position;
    float tileSize;
    int maxTiles;
    int tilesPerRow;
    int selectedIndex;

    // �V�����f�[�^�\���F�e�p�^�[�����O���[�o���J���[�p���b�g��3�F���Q��
    std::vector<std::array<int, 3>> globalColorIndices; // �e�p�^�[�����g�p����O���[�o���J���[�̃C���f�b�N�X

    // ���V�X�e���i�ꎞ�I�ɕ����j
    std::vector<std::array<sf::Color, 3>> colorPalettes; // �����@�\�ێ��̂���

public:
    void setPosition(const sf::Vector2f& pos) {
        this->position = pos;
    }

    void selectPattern(int index);

    TilePalette(float tileSize, sf::Vector2f position)
        : position(position), tileSize(tileSize), maxTiles(64), tilesPerRow(4), selectedIndex(-1) {
        // �����͋�̃p���b�g
    }

    // �V�����֐��F�O���[�o���J���[�C���f�b�N�X��ݒ�
    void setGlobalColorIndices(int patternIndex, const std::array<int, 3>& colorIndices) {
        if (patternIndex >= 0 && patternIndex < globalColorIndices.size()) {
            globalColorIndices[patternIndex] = colorIndices;
        }
    }

    // �V�����֐��F�O���[�o���J���[�C���f�b�N�X���擾
    std::array<int, 3> getGlobalColorIndices(int patternIndex) const {
        if (patternIndex >= 0 && patternIndex < globalColorIndices.size()) {
            return globalColorIndices[patternIndex];
        }
        return {0, 1, 2}; // �f�t�H���g�F�ŏ���3�F
    }

    // �I�����ꂽ�p�^�[���̃O���[�o���J���[�C���f�b�N�X���擾
    std::array<int, 3> getSelectedGlobalColorIndices() const {
        if (selectedIndex >= 0) {
            return getGlobalColorIndices(selectedIndex);
        }
        return {0, 1, 2}; // �f�t�H���g
    }

    // �����̊֐��Q�i�݊����ێ��̂��߁j
    int getPatternCount() const {
        return patterns.size();
    }

    void updatePattern(int index, const std::vector<std::vector<int>>& pattern) {
        if (index >= 0 && index < patterns.size()) {
            std::vector<int> flat;
            for (const auto& row : pattern)
                for (int val : row)
                    flat.push_back(val);
            patterns[index] = flat;
        }
    }

    void updateColorSet(int index, const std::array<sf::Color, 3>& colors) {
        if (index >= 0 && index < colorPalettes.size()) {
            colorPalettes[index] = colors;
        }
    }

    std::vector<int> getPattern(int index) const {
        if (index >= 0 && index < patterns.size()) {
            return patterns[index];
        }
        return std::vector<int>();
    }

    const std::vector<std::vector<int>>& getAllPatterns() const {
        return patterns;
    }

    // �p�^�[���ǉ��i�V�o�[�W�����F�O���[�o���J���[�C���f�b�N�X�w��j
    void addPatternWithGlobalColors(const std::vector<std::vector<int>>& pattern, 
                                   const std::array<int, 3>& globalColorIndices) {
        if (patterns.size() >= maxTiles) return;

        std::vector<int> flatPattern;
        if (pattern.size() != 3 || pattern[0].size() != 3) {
            flatPattern.resize(9, 0);
        } else {
            for (int y = 0; y < pattern.size(); ++y) {
                for (int x = 0; x < pattern[y].size(); ++x) {
                    flatPattern.push_back(pattern[y][x]);
                }
            }
        }

        if (flatPattern.size() != 9) {
            flatPattern.resize(9, 0);
        }

        patterns.push_back(flatPattern);
        this->globalColorIndices.push_back(globalColorIndices);

        // ���V�X�e���p�̃_�~�[�J���[�Z�b�g���ǉ��i�݊����̂��߁j
        std::array<sf::Color, 3> dummyColors = {sf::Color::Red, sf::Color::Green, sf::Color::Blue};
        colorPalettes.push_back(dummyColors);

        selectedIndex = patterns.size() - 1;
    }

    // ������addPattern�i�݊����ێ��j
    void addPattern(const std::vector<std::vector<int>>& pattern, const std::array<sf::Color, 3>& colorSet) {
        // �f�t�H���g�ŃO���[�o���J���[�̍ŏ���3�F���g�p
        addPatternWithGlobalColors(pattern, {0, 1, 2});
        
        // ���V�X�e���̐F���ݒ�
        if (!colorPalettes.empty()) {
            colorPalettes.back() = colorSet;
        }
    }

    const std::vector<int>& getSelectedPattern() const {
        static std::vector<int> emptyPattern(9, 0);
        if (selectedIndex >= 0 && selectedIndex < patterns.size()) {
            return patterns[selectedIndex];
        }
        return emptyPattern;
    }

    int getSelectedIndex() const {
        return selectedIndex;
    }

    bool handleClick(const sf::Vector2i& mousePos) {
        for (int i = 0; i < patterns.size(); ++i) {
            int row = i / tilesPerRow;
            int col = i % tilesPerRow;

            float x = position.x + col * (tileSize + 5);
            float y = position.y + row * (tileSize + 5);

            sf::FloatRect bounds(x, y, tileSize, tileSize);
            if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                selectedIndex = i;
                return true;
            }
        }

        if (patterns.size() < maxTiles) {
            int row = patterns.size() / tilesPerRow;
            int col = patterns.size() % tilesPerRow;

            float x = position.x + col * (tileSize + 5);
            float y = position.y + row * (tileSize + 5);

            sf::FloatRect bounds(x, y, tileSize, tileSize);
            if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                return true;
            }
        }

        return false;
    }

    // �V�����`��֐��F�O���[�o���J���[�p���b�g���g�p
    void drawWithGlobalColors(sf::RenderWindow& window, const std::array<sf::Color, 16>& globalColors) {
        float cellSize = tileSize / 3.0f;

        for (int i = 0; i < patterns.size(); ++i) {
            int row = i / tilesPerRow;
            int col = i % tilesPerRow;

            float x = position.x + col * (tileSize + 5);
            float y = position.y + row * (tileSize + 5);

            // �^�C���w�i
            sf::RectangleShape background(sf::Vector2f(tileSize, tileSize));
            background.setPosition(x, y);
            background.setFillColor(sf::Color(50, 50, 50));
            background.setOutlineThickness(2);
            background.setOutlineColor(selectedIndex == i ? sf::Color::Yellow : sf::Color(100, 100, 100));
            window.draw(background);

            // �p�^�[���`��i�O���[�o���J���[�g�p�j
            const auto& pattern = patterns[i];
            const auto& globalIndices = globalColorIndices[i];

            if (pattern.size() >= 9) {
                for (int cy = 0; cy < 3; ++cy) {
                    for (int cx = 0; cx < 3; ++cx) {
                        int idx = cy * 3 + cx;
                        if (idx < pattern.size()) {
                            int colorIndex = pattern[idx];

                            // �F�C���f�b�N�X�͈̔̓`�F�b�N�i0, 1, 2�̂ݗL���j
                            if (colorIndex >= 0 && colorIndex < 3) {
                                int globalColorIndex = globalIndices[colorIndex];
                                if (globalColorIndex >= 0 && globalColorIndex < 16) {
                                    sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1));
                                    cell.setPosition(x + cx * cellSize + 0.5f, y + cy * cellSize + 0.5f);
                                    cell.setFillColor(globalColors[globalColorIndex]);
                                    window.draw(cell);
                                }
                            }
                        }
                    }
                }
            }
        }

        // "+" �{�^���i�V�K�ǉ��p�j
        if (patterns.size() < maxTiles) {
            int row = patterns.size() / tilesPerRow;
            int col = patterns.size() % tilesPerRow;

            float x = position.x + col * (tileSize + 5);
            float y = position.y + row * (tileSize + 5);

            sf::RectangleShape addButton(sf::Vector2f(tileSize, tileSize));
            addButton.setPosition(x, y);
            addButton.setFillColor(sf::Color(50, 50, 50));
            addButton.setOutlineThickness(1);
            addButton.setOutlineColor(sf::Color(100, 100, 100));
            window.draw(addButton);

            // "+" �L��
            sf::RectangleShape vLine(sf::Vector2f(4, tileSize * 0.6f));
            vLine.setPosition(x + tileSize / 2 - 2, y + tileSize * 0.2f);
            vLine.setFillColor(sf::Color(200, 200, 200));
            window.draw(vLine);

            sf::RectangleShape hLine(sf::Vector2f(tileSize * 0.6f, 4));
            hLine.setPosition(x + tileSize * 0.2f, y + tileSize / 2 - 2);
            hLine.setFillColor(sf::Color(200, 200, 200));
            window.draw(hLine);
        }
    }

    // �����̕`��֐��i�݊����ێ��j
    void draw(sf::RenderWindow& window, const std::vector<std::array<sf::Color, 3>>& allColorSets) {
        // ���V�X�e���ł̕`��i����݊����̂��߁j
        float cellSize = tileSize / 3.0f;

        for (int i = 0; i < patterns.size(); ++i) {
            int row = i / tilesPerRow;
            int col = i % tilesPerRow;

            float x = position.x + col * (tileSize + 5);
            float y = position.y + row * (tileSize + 5);

            sf::RectangleShape background(sf::Vector2f(tileSize, tileSize));
            background.setPosition(x, y);
            background.setFillColor(sf::Color(50, 50, 50));
            background.setOutlineThickness(2);
            background.setOutlineColor(selectedIndex == i ? sf::Color::Yellow : sf::Color(100, 100, 100));
            window.draw(background);

            const auto& pattern = patterns[i];
            const auto& colorSet = allColorSets[i];

            if (pattern.size() >= 9) {
                for (int cy = 0; cy < 3; ++cy) {
                    for (int cx = 0; cx < 3; ++cx) {
                        int idx = cy * 3 + cx;
                        if (idx < pattern.size()) {
                            int colorIndex = pattern[idx];

                            if (colorIndex >= 0 && colorIndex < colorSet.size()) {
                                sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1));
                                cell.setPosition(x + cx * cellSize + 0.5f, y + cy * cellSize + 0.5f);
                                cell.setFillColor(colorSet[colorIndex]);
                                window.draw(cell);
                            }
                        }
                    }
                }
            }
        }

        // "+" �{�^���`��͓���
        if (patterns.size() < maxTiles) {
            int row = patterns.size() / tilesPerRow;
            int col = patterns.size() % tilesPerRow;

            float x = position.x + col * (tileSize + 5);
            float y = position.y + row * (tileSize + 5);

            sf::RectangleShape addButton(sf::Vector2f(tileSize, tileSize));
            addButton.setPosition(x, y);
            addButton.setFillColor(sf::Color(50, 50, 50));
            addButton.setOutlineThickness(1);
            addButton.setOutlineColor(sf::Color(100, 100, 100));
            window.draw(addButton);

            sf::RectangleShape vLine(sf::Vector2f(4, tileSize * 0.6f));
            vLine.setPosition(x + tileSize / 2 - 2, y + tileSize * 0.2f);
            vLine.setFillColor(sf::Color(200, 200, 200));
            window.draw(vLine);

            sf::RectangleShape hLine(sf::Vector2f(tileSize * 0.6f, 4));
            hLine.setPosition(x + tileSize * 0.2f, y + tileSize / 2 - 2);
            hLine.setFillColor(sf::Color(200, 200, 200));
            window.draw(hLine);
        }
    }

    // ���V�X�e���֐��i�݊����ێ��j
    std::array<sf::Color, 3>& getSelectedColorSet() {
        static std::array<sf::Color, 3> dummy = {
            sf::Color::Red, sf::Color::Green, sf::Color::Blue
        };
        if (selectedIndex >= 0 && selectedIndex < colorPalettes.size()) {
            return colorPalettes[selectedIndex];
        }
        return dummy;
    }

    std::array<sf::Color, 3>& getColorSet(int index) {
        static std::array<sf::Color, 3> defaultColors = { sf::Color::Red, sf::Color::Green, sf::Color::Blue };
        if (index >= 0 && index < colorPalettes.size()) {
            return colorPalettes[index];
        }
        return defaultColors;
    }

    const std::vector<std::array<sf::Color, 3>>& getAllColorPalettes() const {
        return colorPalettes;
    }

    const std::array<sf::Color, 3>& getSelectedColorSet() const {
        static std::array<sf::Color, 3> dummy = {
            sf::Color::Red, sf::Color::Green, sf::Color::Blue
        };
        if (selectedIndex >= 0 && selectedIndex < colorPalettes.size()) {
            return colorPalettes[selectedIndex];
        }
        return dummy;
    }

    void loadPatterns(const std::vector<std::vector<int>>& newPatterns,
                     const std::vector<std::array<sf::Color, 3>>& newColorSets) {
        patterns = newPatterns;
        colorPalettes = newColorSets;
        
        // �O���[�o���J���[�C���f�b�N�X�̃f�t�H���g�ݒ�
        globalColorIndices.clear();
        for (int i = 0; i < patterns.size(); ++i) {
            globalColorIndices.push_back({0, 1, 2}); // �f�t�H���g�F�ŏ���3�F
        }
        
        selectedIndex = -1;
    }


   

    void clearPatterns() {
        patterns.clear();
        colorPalettes.clear();
        globalColorIndices.clear();
        selectedIndex = -1;
    }
};