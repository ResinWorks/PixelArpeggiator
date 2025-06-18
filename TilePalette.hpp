
//===== TilePalette.hpp =====
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

	//�Ǝ���3�F�p���b�g�p_�ǉ�
	std::vector<std::array<sf::Color, 3>> colorPalettes;

public:

	void setPosition(const sf::Vector2f& pos) {
		this->position = pos;
	}

	void selectPattern(int index); // �� �ǉ�
	/*
	void selectPattern(int index) {
		if (index >= 0 && index < static_cast<int>(patterns.size())) {
			selectedIndex = index;
		}
	}
	*/
	TilePalette(float tileSize, sf::Vector2f position)
		: position(position), tileSize(tileSize), maxTiles(64), tilesPerRow(4), selectedIndex(-1) {
		// �����͋�̃p���b�g
	}

	// �ǉ�����֐�

	int getPatternCount() const {
		return patterns.size();
	}

	// �p�^�[���̍X�V(�蓮�X�V�p�j
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
	/*
	*/
	//�ǉ��@�S�p�^�[���擾

	std::vector<int> getPattern(int index) const {
		if (index >= 0 && index < patterns.size()) {
			return patterns[index];
		}
		return std::vector<int>(); // ��̃p�^�[����Ԃ�
	}

	// �ǉ��@�S�p�^�[���擾
	const std::vector<std::vector<int>>& getAllPatterns() const {
		return patterns;
	}

	// 3x3�p�^�[�����p���b�g�ɒǉ�
	void addPattern(const std::vector<std::vector<int>>& pattern, const std::array<sf::Color, 3>& colorSet) {
		if (patterns.size() >= maxTiles) return;

		std::vector<int> flatPattern;
		// �p�^�[���̃T�C�Y���`�F�b�N
		if (pattern.size() != 3 || pattern[0].size() != 3) {
			// �T�C�Y���Ⴄ�ꍇ�͈��S�ȃf�t�H���g�l��ݒ�
			flatPattern.resize(9, 0);
		}
		else {
			// 2��������1�����ɕϊ�
			for (int y = 0; y < pattern.size(); ++y) {
				for (int x = 0; x < pattern[y].size(); ++x) {
					flatPattern.push_back(pattern[y][x]);
				}
			}
		}

		// �p�^�[�����������T�C�Y�ł��邱�Ƃ��m�F
		if (flatPattern.size() != 9) {
			flatPattern.resize(9, 0);
		}

		patterns.push_back(flatPattern);

		//�Ǝ���3�F�p���b�g�p_�ǉ�
		colorPalettes.push_back(colorSet); // �F���ۑ�
		// �ǉ������p�^�[����I��
		selectedIndex = patterns.size() - 1;
	}

	// �I�����ꂽ�p�^�[�����擾
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

	// �N���b�N����
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

		// "+" �{�^���i�V�K�ǉ��p�j
		if (patterns.size() < maxTiles) {
			int row = patterns.size() / tilesPerRow;
			int col = patterns.size() % tilesPerRow;

			float x = position.x + col * (tileSize + 5);
			float y = position.y + row * (tileSize + 5);

			sf::FloatRect bounds(x, y, tileSize, tileSize);
			if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
				return true; // �V�K�ǉ����[�h - main.cpp�ŏ���
			}
		}

		return false;
	}

	//void draw(sf::RenderWindow& window, const std::array<sf::Color, 3>& colorSet)
	void draw(sf::RenderWindow& window, const std::vector<std::array<sf::Color, 3>>& allColorSets)
	{
		float cellSize = tileSize / 3.0f;

		// �����̃p�^�[���`��
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

			// �p�^�[���`��
			const auto& pattern = patterns[i];
			const auto& colorSet = allColorSets[i]; // �F�Z�b�g���擾

			if (pattern.size() >= 9) {  // �p�^�[���T�C�Y�`�F�b�N
				for (int cy = 0; cy < 3; ++cy) {
					for (int cx = 0; cx < 3; ++cx) {
						int idx = cy * 3 + cx;
						if (idx < pattern.size()) {
							int colorIndex = pattern[idx];

							// �F�C���f�b�N�X�͈̔̓`�F�b�N
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


	std::array<sf::Color, 3>& getSelectedColorSet() {
		static std::array<sf::Color, 3> dummy = {
			sf::Color::Red, sf::Color::Green, sf::Color::Blue
		};
		if (selectedIndex >= 0 && selectedIndex < colorPalettes.size()) {
			return colorPalettes[selectedIndex]; // �� ��const�Q�Ƃ�Ԃ�
		}
		return dummy;
	}

	 std::array<sf::Color, 3>& getColorSet(int index)  {
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

	//-------------�Z�[�u�E���[�h�p�ǉ��֐�------------------------
	void loadPatterns(const std::vector<std::vector<int>>& newPatterns,
		const std::vector<std::array<sf::Color, 3>>& newColorSets) {
		patterns = newPatterns;
		colorPalettes = newColorSets;
		selectedIndex = -1; // ������Ԃɖ߂��i�K�v�Ȃ�j
	}

	
};
