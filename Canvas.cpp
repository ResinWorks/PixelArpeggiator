#include "Canvas.hpp"
#include "CanvasView.hpp"  // ������CanvasView���C���N���[�h
#include <iostream>

// ===== �����̃��\�b�h�����i�ύX�Ȃ��j =====

void Canvas::initializeRenderTexture() {
    if (!renderTexture.create(width * tileSize, height * tileSize)) {
        std::cerr << "Error: Failed to create render texture" << std::endl;
        return;
    }
    isInitialized = true;
    isDirty = true;
}

void Canvas::handleClick(const sf::Vector2i& mousePos, int selectedTileIndex) {
    if (selectedTileIndex < 0) return;

    sf::Vector2f localPos = static_cast<sf::Vector2f>(mousePos) - position;
    int x = static_cast<int>(localPos.x) / tileSize;
    int y = static_cast<int>(localPos.y) / tileSize;

    if (x >= 0 && x < width && y >= 0 && y < height) {
        if (tiles[y][x] != selectedTileIndex) {
            tiles[y][x] = selectedTileIndex;
            isDirty = true;
        }
    }
}

// ������renderToTexture ���\�b�h���X�V
// renderToTexture ���\�b�h�̏C���i�ȒP�ȕύX�̂݁j
void Canvas::renderToTexture(const std::vector<std::vector<int>>& patterns,
    const std::vector<std::array<sf::Color, 3>>& colorPalettes,
    bool showGrid, float spacing, float shrink) {

    // �w�i�F�̐ݒ�i�ύX�Ȃ��j
    renderTexture.clear(sf::Color(40, 40, 40));

    // �O���b�h�`��i�S�̃O���b�h - �ύX�Ȃ��j
    if (showGrid) {
        sf::VertexArray lines(sf::Lines);

        // �c��
        for (int x = 0; x <= width; ++x) {
            lines.append(sf::Vertex(sf::Vector2f(x * tileSize, 0), sf::Color(70, 70, 70)));
            lines.append(sf::Vertex(sf::Vector2f(x * tileSize, height * tileSize), sf::Color(70, 70, 70)));
        }

        // ����
        for (int y = 0; y <= height; ++y) {
            lines.append(sf::Vertex(sf::Vector2f(0, y * tileSize), sf::Color(70, 70, 70)));
            lines.append(sf::Vertex(sf::Vector2f(width * tileSize, y * tileSize), sf::Color(70, 70, 70)));
        }

        renderTexture.draw(lines);
    }

    // ===== �C���F�^�C���`��i�^�C�����E�O���b�h�Ή��j =====
    float cellSize = tileSize / 3.0f;
    sf::RectangleShape cell;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int tileIndex = tiles[y][x];

            // �����ȃ^�C���̏ꍇ�̓X�L�b�v
            if (tileIndex < 0) continue;

            // �͈̓`�F�b�N
            if (tileIndex >= patterns.size() || tileIndex >= colorPalettes.size()) {
                continue;
            }

            const auto& pattern = patterns[tileIndex];
            const auto& colorSet = colorPalettes[tileIndex];

            // ===== �V�����F�^�C�����E�O���b�h���̕`�� =====
            if (useTileGridColor && spacing > 0.0f) {
                // �^�C���S�̗̂̈���^�C�������O���b�h�F�œh��Ԃ�
                sf::RectangleShape tileBackground(sf::Vector2f(tileSize, tileSize));
                tileBackground.setPosition(x * tileSize, y * tileSize);
                tileBackground.setFillColor(tileGridColor);
                renderTexture.draw(tileBackground);

                // �����̕`��̈���v�Z�ispacing�����������Ɂj
                float borderWidth = tileSize * spacing * 0.5f;  // spacing �����E���̕��Ƃ��Ďg�p
                float innerTileSize = tileSize - (borderWidth * 2);
                float innerCellSize = innerTileSize / 3.0f;

                // 3x3�p�^�[��������̗̈�ɕ`��
                if (pattern.size() >= 9) {
                    for (int cy = 0; cy < 3; ++cy) {
                        for (int cx = 0; cx < 3; ++cx) {
                            int colorIndex = pattern[cy * 3 + cx];
                            if (colorIndex >= 0 && colorIndex < 3) {
                                // �Z���T�C�Y�� shrink �Œ���
                                float adjustedCellSize = innerCellSize * shrink;
                                float cellCenterOffset = (innerCellSize - adjustedCellSize) * 0.5f;

                                cell.setSize(sf::Vector2f(adjustedCellSize, adjustedCellSize));
                                cell.setPosition(
                                    x * tileSize + borderWidth + cx * innerCellSize + cellCenterOffset,
                                    y * tileSize + borderWidth + cy * innerCellSize + cellCenterOffset
                                );
                                cell.setFillColor(colorSet[colorIndex]);
                                renderTexture.draw(cell);
                            }
                        }
                    }
                }
            }
            else {
                // spacing = 0 �̏ꍇ�͏]���ʂ�̕`��
                if (pattern.size() >= 9) {
                    for (int cy = 0; cy < 3; ++cy) {
                        for (int cx = 0; cx < 3; ++cx) {
                            int colorIndex = pattern[cy * 3 + cx];
                            if (colorIndex >= 0 && colorIndex < 3) {
                                float adjustedCellSize = cellSize * shrink;
                                float cellCenterOffset = (cellSize - adjustedCellSize) * 0.5f;

                                cell.setSize(sf::Vector2f(adjustedCellSize, adjustedCellSize));
                                cell.setPosition(
                                    x * tileSize + cx * cellSize + cellCenterOffset,
                                    y * tileSize + cy * cellSize + cellCenterOffset
                                );
                                cell.setFillColor(colorSet[colorIndex]);
                                renderTexture.draw(cell);
                            }
                        }
                    }
                }
            }
        }
    }

    renderTexture.display();
}

void Canvas::draw(sf::RenderWindow& window,
    const std::vector<std::vector<int>>& patterns,
    const std::vector<std::array<sf::Color, 3>>& colorPalettes,
    bool showGrid,
    float spacing,
    float shrink) {

    if (!isInitialized) {
        initializeRenderTexture();
    }

    bool settingsChanged = (showGrid != lastShowGrid ||
        spacing != lastSpacing ||
        shrink != lastShrink);

    if (isDirty || settingsChanged) {
        renderToTexture(patterns, colorPalettes, showGrid, spacing, shrink);
        isDirty = false;
        lastShowGrid = showGrid;
        lastSpacing = spacing;
        lastShrink = shrink;
    }

    if (isInitialized) {
        sf::Sprite sprite(renderTexture.getTexture());
        sprite.setPosition(position);
        window.draw(sprite);

        sf::RectangleShape border(sf::Vector2f(width * tileSize, height * tileSize));
        border.setPosition(position);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1);
        border.setOutlineColor(sf::Color(100, 100, 100));
        window.draw(border);
    }
}

void Canvas::eraseTile(const sf::Vector2i& position) {
    int tileX = (position.x - this->position.x) / tileSize;
    int tileY = (position.y - this->position.y) / tileSize;

    if (tileX >= 0 && tileX < width && tileY >= 0 && tileY < height) {
        tiles[tileY][tileX] = -1;
        isDirty = true;
    }
}

void Canvas::setTile(const sf::Vector2i& position, int tileIndex) {
    int tileX = (position.x - this->position.x) / tileSize;
    int tileY = (position.y - this->position.y) / tileSize;

    if (tileX >= 0 && tileX < width && tileY >= 0 && tileY < height) {
        tiles[tileY][tileX] = tileIndex;
        isDirty = true;
    }
}

// ===== CanvasView�Ή����\�b�h�̎����i�����ɒǉ��j =====

void Canvas::drawWithView(sf::RenderWindow& window,
    const CanvasView& view,
    const std::vector<std::vector<int>>& patterns,
    const std::vector<std::array<sf::Color, 3>>& colorPalettes,
    bool showGrid,
    float spacing,
    float shrink) {

    if (!isInitialized) {
        initializeRenderTexture();
    }

    bool settingsChanged = (showGrid != lastShowGrid ||
        spacing != lastSpacing ||
        shrink != lastShrink);

    if (isDirty || settingsChanged) {
        renderToTexture(patterns, colorPalettes, showGrid, spacing, shrink);
        isDirty = false;
        lastShowGrid = showGrid;
        lastSpacing = spacing;
        lastShrink = shrink;
    }

    if (isInitialized) {
        sf::Sprite sprite(renderTexture.getTexture());
        sprite.setPosition(position);

        // CanvasView�̕ϊ���K�p
        sf::RenderStates states;
        states.transform = view.getTransform();

        window.draw(sprite, states);

        // ���E����`��
        sf::RectangleShape border(sf::Vector2f(width * tileSize, height * tileSize));
        border.setPosition(position);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(1.0f / view.getZoom());
        border.setOutlineColor(sf::Color(100, 100, 100));
        window.draw(border, states);
    }
}

bool Canvas::containsInView(const CanvasView& view, const sf::Vector2i& screenPos) const {
    sf::Vector2i canvasPos = view.screenToCanvas(screenPos);
    sf::FloatRect canvasRect(position.x, position.y, width * tileSize, height * tileSize);
    return canvasRect.contains(static_cast<sf::Vector2f>(canvasPos));
}

void Canvas::handleClickInView(const CanvasView& view, const sf::Vector2i& screenPos, int patternIndex) {
    if (patternIndex < 0) return;

    if (!containsInView(view, screenPos)) return;

    sf::Vector2i tileIndex = screenToTileIndex(view, screenPos);

    if (tileIndex.x >= 0 && tileIndex.x < width &&
        tileIndex.y >= 0 && tileIndex.y < height) {

        if (tiles[tileIndex.y][tileIndex.x] != patternIndex) {
            tiles[tileIndex.y][tileIndex.x] = patternIndex;
            isDirty = true;
        }
    }
}

void Canvas::eraseTileInView(const CanvasView& view, const sf::Vector2i& screenPos) {
    if (!containsInView(view, screenPos)) return;

    sf::Vector2i tileIndex = screenToTileIndex(view, screenPos);

    if (tileIndex.x >= 0 && tileIndex.x < width &&
        tileIndex.y >= 0 && tileIndex.y < height) {

        if (tiles[tileIndex.y][tileIndex.x] != -1) {
            tiles[tileIndex.y][tileIndex.x] = -1;
            isDirty = true;
        }
    }
}

/*

Canvas::PerformanceInfo Canvas::getPerformanceInfo(const CanvasView& view) const {
    PerformanceInfo info;
    info.totalTiles = width * height;
    info.zoomLevel = view.getZoom();

    sf::FloatRect visibleArea = view.getVisibleCanvasArea();

    int startX = std::max(0, static_cast<int>((visibleArea.left - position.x) / tileSize));
    int startY = std::max(0, static_cast<int>((visibleArea.top - position.y) / tileSize));
    int endX = std::min(width, static_cast<int>((visibleArea.left + visibleArea.width - position.x) / tileSize) + 1);
    int endY = std::min(height, static_cast<int>((visibleArea.top + visibleArea.height - position.y) / tileSize) + 1);

    info.visibleTiles = (endX - startX) * (endY - startY);

    info.drawnTiles = 0;
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height && tiles[y][x] >= 0) {
                info.drawnTiles++;
            }
        }
    }

    return info;
}
*/

sf::Vector2i Canvas::screenToTileIndex(const CanvasView& view, const sf::Vector2i& screenPos) const {
    sf::Vector2i canvasPos = view.screenToCanvas(screenPos);
    sf::Vector2f localPos = static_cast<sf::Vector2f>(canvasPos) - position;
    int tileX = static_cast<int>(localPos.x) / tileSize;
    int tileY = static_cast<int>(localPos.y) / tileSize;
    return sf::Vector2i(tileX, tileY);
}

/**
 * �L�����o�X���摜�t�@�C���Ƃ��ďo��
 * ���݂̃L�����o�X�T�C�Y�ŏo��
 */
bool Canvas::exportToImage(const std::string& filename,
    const std::vector<std::vector<int>>& patterns,
    const std::vector<std::array<sf::Color, 3>>& colorPalettes,
    bool showGrid,
    float spacing,
    float shrink) {

    // �o�͗pRenderTexture���쐬
    sf::RenderTexture outputTexture;
    if (!outputTexture.create(width * tileSize, height * tileSize)) {
        std::cerr << "Error: Failed to create output texture for image export" << std::endl;
        return false;
    }

    // �L�����o�X���e���o�͗p�e�N�X�`���ɕ`��
    renderToOutputTexture(outputTexture, patterns, colorPalettes, showGrid, spacing, shrink);

    // �e�N�X�`������摜���擾���ĕۑ�
    sf::Image outputImage = outputTexture.getTexture().copyToImage();

    // �t�@�C���ۑ�
    if (outputImage.saveToFile(filename)) {
        std::cout << "Image exported successfully: " << filename << std::endl;
        std::cout << "Size: " << width * tileSize << "x" << height * tileSize << " pixels" << std::endl;
        return true;
    }
    else {
        std::cerr << "Error: Failed to save image: " << filename << std::endl;
        return false;
    }
}


/**
 * �摜�o�͗p�̓����`�揈��
 * ������renderToTexture()���Q�l�ɁA�o�͐�p�̕`������s
 */


 // �摜�o�͗p�̕`�揈�������l�ɏC��
void Canvas::renderToOutputTexture(sf::RenderTexture& outputTexture,
    const std::vector<std::vector<int>>& patterns,
    const std::vector<std::array<sf::Color, 3>>& colorPalettes,
    bool showGrid,
    float spacing,
    float shrink,
    float scale) {

    // �w�i�F�̐ݒ�i�ύX�Ȃ��j
    outputTexture.clear(sf::Color::Transparent);

    // �X�P�[���ϊ���K�p�i�ύX�Ȃ��j
    sf::View outputView;
    if (scale != 1.0f) {
        outputView.setSize(width * tileSize / scale, height * tileSize / scale);
        outputView.setCenter((width * tileSize) / 2.0f, (height * tileSize) / 2.0f);
        outputTexture.setView(outputView);
    }

    // �O���b�h�`��i�S�̃O���b�h - �ύX�Ȃ��j
    if (showGrid) {
        sf::VertexArray lines(sf::Lines);
        sf::Color gridLineColor = sf::Color(70, 70, 70);

        for (int x = 0; x <= width; ++x) {
            lines.append(sf::Vertex(sf::Vector2f(x * tileSize, 0), gridLineColor));
            lines.append(sf::Vertex(sf::Vector2f(x * tileSize, height * tileSize), gridLineColor));
        }

        for (int y = 0; y <= height; ++y) {
            lines.append(sf::Vertex(sf::Vector2f(0, y * tileSize), gridLineColor));
            lines.append(sf::Vertex(sf::Vector2f(width * tileSize, y * tileSize), gridLineColor));
        }

        outputTexture.draw(lines);
    }

    // ===== �^�C���`��i�摜�o�͗p�A���������j =====
    float cellSize = tileSize / 3.0f;
    sf::RectangleShape cell;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int tileIndex = tiles[y][x];

            if (tileIndex < 0) continue;
            if (tileIndex >= patterns.size() || tileIndex >= colorPalettes.size()) continue;

            const auto& pattern = patterns[tileIndex];
            const auto& colorSet = colorPalettes[tileIndex];

            // �^�C�����E�O���b�h���̕`��i�������W�b�N�j
            if (useTileGridColor && spacing > 0.0f) {
                // �^�C���S�̂��O���b�h�F�œh��Ԃ�
                sf::RectangleShape tileBackground(sf::Vector2f(tileSize, tileSize));
                tileBackground.setPosition(x * tileSize, y * tileSize);
                tileBackground.setFillColor(tileGridColor);
                outputTexture.draw(tileBackground);

                // �����̕`��̈�
                float borderWidth = tileSize * spacing * 0.5f;
                float innerTileSize = tileSize - (borderWidth * 2);
                float innerCellSize = innerTileSize / 3.0f;

                // 3x3�p�^�[��������ɕ`��
                if (pattern.size() >= 9) {
                    for (int cy = 0; cy < 3; ++cy) {
                        for (int cx = 0; cx < 3; ++cx) {
                            int colorIndex = pattern[cy * 3 + cx];
                            if (colorIndex >= 0 && colorIndex < 3) {
                                float adjustedCellSize = innerCellSize * shrink;
                                float cellCenterOffset = (innerCellSize - adjustedCellSize) * 0.5f;

                                cell.setSize(sf::Vector2f(adjustedCellSize, adjustedCellSize));
                                cell.setPosition(
                                    x * tileSize + borderWidth + cx * innerCellSize + cellCenterOffset,
                                    y * tileSize + borderWidth + cy * innerCellSize + cellCenterOffset
                                );
                                cell.setFillColor(colorSet[colorIndex]);
                                outputTexture.draw(cell);
                            }
                        }
                    }
                }
            }
            else {
                // spacing = 0 �̏]���`��
                if (pattern.size() >= 9) {
                    for (int cy = 0; cy < 3; ++cy) {
                        for (int cx = 0; cx < 3; ++cx) {
                            int colorIndex = pattern[cy * 3 + cx];
                            if (colorIndex >= 0 && colorIndex < 3) {
                                float adjustedCellSize = cellSize * shrink;
                                float cellCenterOffset = (cellSize - adjustedCellSize) * 0.5f;

                                cell.setSize(sf::Vector2f(adjustedCellSize, adjustedCellSize));
                                cell.setPosition(
                                    x * tileSize + cx * cellSize + cellCenterOffset,
                                    y * tileSize + cy * cellSize + cellCenterOffset
                                );
                                cell.setFillColor(colorSet[colorIndex]);
                                outputTexture.draw(cell);
                            }
                        }
                    }
                }
            }
        }
    }

    outputTexture.display();
}
