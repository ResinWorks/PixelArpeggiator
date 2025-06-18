
//===== TilePalette.hpp =====
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

class TilePalette {
private:
	std::vector<std::vector<int>> patterns; // 各タイルのパターン（3x3の色インデックス）
	sf::Vector2f position;
	float tileSize;
	int maxTiles;
	int tilesPerRow;
	int selectedIndex;

	//独自の3色パレット用_追加
	std::vector<std::array<sf::Color, 3>> colorPalettes;

public:

	void setPosition(const sf::Vector2f& pos) {
		this->position = pos;
	}

	void selectPattern(int index); // ← 追加
	/*
	void selectPattern(int index) {
		if (index >= 0 && index < static_cast<int>(patterns.size())) {
			selectedIndex = index;
		}
	}
	*/
	TilePalette(float tileSize, sf::Vector2f position)
		: position(position), tileSize(tileSize), maxTiles(64), tilesPerRow(4), selectedIndex(-1) {
		// 初期は空のパレット
	}

	// 追加する関数

	int getPatternCount() const {
		return patterns.size();
	}

	// パターンの更新(手動更新用）
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
	//追加　全パターン取得

	std::vector<int> getPattern(int index) const {
		if (index >= 0 && index < patterns.size()) {
			return patterns[index];
		}
		return std::vector<int>(); // 空のパターンを返す
	}

	// 追加　全パターン取得
	const std::vector<std::vector<int>>& getAllPatterns() const {
		return patterns;
	}

	// 3x3パターンをパレットに追加
	void addPattern(const std::vector<std::vector<int>>& pattern, const std::array<sf::Color, 3>& colorSet) {
		if (patterns.size() >= maxTiles) return;

		std::vector<int> flatPattern;
		// パターンのサイズをチェック
		if (pattern.size() != 3 || pattern[0].size() != 3) {
			// サイズが違う場合は安全なデフォルト値を設定
			flatPattern.resize(9, 0);
		}
		else {
			// 2次元から1次元に変換
			for (int y = 0; y < pattern.size(); ++y) {
				for (int x = 0; x < pattern[y].size(); ++x) {
					flatPattern.push_back(pattern[y][x]);
				}
			}
		}

		// パターンが正しいサイズであることを確認
		if (flatPattern.size() != 9) {
			flatPattern.resize(9, 0);
		}

		patterns.push_back(flatPattern);

		//独自の3色パレット用_追加
		colorPalettes.push_back(colorSet); // 色も保存
		// 追加したパターンを選択
		selectedIndex = patterns.size() - 1;
	}

	// 選択されたパターンを取得
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

	// クリック処理
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

		// "+" ボタン（新規追加用）
		if (patterns.size() < maxTiles) {
			int row = patterns.size() / tilesPerRow;
			int col = patterns.size() % tilesPerRow;

			float x = position.x + col * (tileSize + 5);
			float y = position.y + row * (tileSize + 5);

			sf::FloatRect bounds(x, y, tileSize, tileSize);
			if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
				return true; // 新規追加モード - main.cppで処理
			}
		}

		return false;
	}

	//void draw(sf::RenderWindow& window, const std::array<sf::Color, 3>& colorSet)
	void draw(sf::RenderWindow& window, const std::vector<std::array<sf::Color, 3>>& allColorSets)
	{
		float cellSize = tileSize / 3.0f;

		// 既存のパターン描画
		for (int i = 0; i < patterns.size(); ++i) {
			int row = i / tilesPerRow;
			int col = i % tilesPerRow;

			float x = position.x + col * (tileSize + 5);
			float y = position.y + row * (tileSize + 5);

			// タイル背景
			sf::RectangleShape background(sf::Vector2f(tileSize, tileSize));
			background.setPosition(x, y);
			background.setFillColor(sf::Color(50, 50, 50));
			background.setOutlineThickness(2);
			background.setOutlineColor(selectedIndex == i ? sf::Color::Yellow : sf::Color(100, 100, 100));
			window.draw(background);

			// パターン描画
			const auto& pattern = patterns[i];
			const auto& colorSet = allColorSets[i]; // 色セットを取得

			if (pattern.size() >= 9) {  // パターンサイズチェック
				for (int cy = 0; cy < 3; ++cy) {
					for (int cx = 0; cx < 3; ++cx) {
						int idx = cy * 3 + cx;
						if (idx < pattern.size()) {
							int colorIndex = pattern[idx];

							// 色インデックスの範囲チェック
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

		// "+" ボタン（新規追加用）
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

			// "+" 記号
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
			return colorPalettes[selectedIndex]; // ★ 非const参照を返す
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

	//-------------セーブ・ロード用追加関数------------------------
	void loadPatterns(const std::vector<std::vector<int>>& newPatterns,
		const std::vector<std::array<sf::Color, 3>>& newColorSets) {
		patterns = newPatterns;
		colorPalettes = newColorSets;
		selectedIndex = -1; // 初期状態に戻す（必要なら）
	}

	
};
