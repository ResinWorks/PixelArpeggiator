#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <iostream>

// --- 型別名定義 ---
using PatternData = std::vector<int>;
using ColorSet = std::array<sf::Color, 3>;
using CanvasData = std::vector<std::vector<int>>;



// --- シンプルなバイナリ形式でのセーブ関数 ---
void saveProject(const std::string& filename,
	const std::vector<PatternData>& patterns,
	const std::vector<ColorSet>& colorSets,
	const CanvasData& canvas) {
	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs.is_open()) {
		std::cerr << "ファイルを開けませんでした: " << filename << std::endl;
		return;
	}

	// パターン数を保存
	size_t patternCount = patterns.size();
	ofs.write(reinterpret_cast<const char*>(&patternCount), sizeof(patternCount));

	// 各パターンと色を保存
	for (size_t i = 0; i < patterns.size(); ++i) {
		// パターンデータ（9要素固定）
		for (int j = 0; j < 9; ++j) {
			int value = (j < patterns[i].size()) ? patterns[i][j] : 0;
			ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
		}

		// 色データ（RGB各色）
		for (int c = 0; c < 3; ++c) {
			sf::Uint8 r = (i < colorSets.size()) ? colorSets[i][c].r : 255;
			sf::Uint8 g = (i < colorSets.size()) ? colorSets[i][c].g : 255;
			sf::Uint8 b = (i < colorSets.size()) ? colorSets[i][c].b : 255;
			ofs.write(reinterpret_cast<const char*>(&r), sizeof(r));
			ofs.write(reinterpret_cast<const char*>(&g), sizeof(g));
			ofs.write(reinterpret_cast<const char*>(&b), sizeof(b));
		}
	}

	// キャンバスサイズを保存
	size_t canvasHeight = canvas.size();
	size_t canvasWidth = (canvasHeight > 0) ? canvas[0].size() : 0;
	ofs.write(reinterpret_cast<const char*>(&canvasHeight), sizeof(canvasHeight));
	ofs.write(reinterpret_cast<const char*>(&canvasWidth), sizeof(canvasWidth));

	// キャンバスデータを保存
	for (const auto& row : canvas) {
		for (size_t x = 0; x < canvasWidth; ++x) {
			int value = (x < row.size()) ? row[x] : -1;
			ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
		}
	}

	ofs.close();
	std::cout << "プロジェクトを保存しました: " << filename << std::endl;
}

// --- シンプルなバイナリ形式でのロード関数 ---
bool loadProject(const std::string& filename,
	std::vector<PatternData>& patternsOut,
	std::vector<ColorSet>& colorSetsOut,
	CanvasData& canvasOut) {
	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs.is_open()) {
		std::cerr << "ファイルが開けません: " << filename << std::endl;
		return false;
	}

	try {
		// 結果をクリア
		patternsOut.clear();
		colorSetsOut.clear();
		canvasOut.clear();

		// パターン数を読み込み
		size_t patternCount;
		ifs.read(reinterpret_cast<char*>(&patternCount), sizeof(patternCount));

		if (ifs.fail() || patternCount > 100) { // 妥当性チェック
			std::cerr << "不正なパターン数: " << patternCount << std::endl;
			return false;
		}

		std::cout << "読み込むパターン数: " << patternCount << std::endl;

		// 各パターンと色を読み込み
		for (size_t i = 0; i < patternCount; ++i) {
			// パターンデータ（9要素）
			std::vector<int> pattern(9);
			for (int j = 0; j < 9; ++j) {
				ifs.read(reinterpret_cast<char*>(&pattern[j]), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "パターン" << i << "の読み込みエラー" << std::endl;
					return false;
				}
			}
			patternsOut.push_back(pattern);

			// 色データ（3色）
			std::array<sf::Color, 3> colors;
			for (int c = 0; c < 3; ++c) {
				sf::Uint8 r, g, b;
				ifs.read(reinterpret_cast<char*>(&r), sizeof(r));
				ifs.read(reinterpret_cast<char*>(&g), sizeof(g));
				ifs.read(reinterpret_cast<char*>(&b), sizeof(b));
				if (ifs.fail()) {
					std::cerr << "色" << i << "-" << c << "の読み込みエラー" << std::endl;
					return false;
				}
				colors[c] = sf::Color(r, g, b);
			}
			colorSetsOut.push_back(colors);
		}

		// キャンバスサイズを読み込み
		size_t canvasHeight, canvasWidth;
		ifs.read(reinterpret_cast<char*>(&canvasHeight), sizeof(canvasHeight));
		ifs.read(reinterpret_cast<char*>(&canvasWidth), sizeof(canvasWidth));

		if (ifs.fail() || canvasHeight > 1000 || canvasWidth > 1000) { // 妥当性チェック
			std::cerr << "不正なキャンバスサイズ: " << canvasWidth << "x" << canvasHeight << std::endl;
			return false;
		}

		std::cout << "キャンバスサイズ: " << canvasWidth << "x" << canvasHeight << std::endl;

		// キャンバスデータを読み込み
		canvasOut.resize(canvasHeight);
		for (size_t y = 0; y < canvasHeight; ++y) {
			canvasOut[y].resize(canvasWidth);
			for (size_t x = 0; x < canvasWidth; ++x) {
				ifs.read(reinterpret_cast<char*>(&canvasOut[y][x]), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "キャンバス(" << x << "," << y << ")の読み込みエラー" << std::endl;
					return false;
				}
			}
		}

		std::cout << "読み込み完了: "
			<< patternsOut.size() << "パターン, "
			<< colorSetsOut.size() << "色セット, "
			<< canvasOut.size() << "キャンバス行" << std::endl;

		return true;

	}
	catch (const std::exception& e) {
		std::cerr << "読み込み中にエラーが発生しました: " << e.what() << std::endl;
		return false;
	}
}

// --- テキスト形式（JSON風）でのセーブ関数（デバッグ用） ---
void saveProjectText(const std::string& filename,
	const std::vector<PatternData>& patterns,
	const std::vector<ColorSet>& colorSets,
	const CanvasData& canvas) {
	std::ofstream ofs(filename);
	if (!ofs.is_open()) {
		std::cerr << "ファイルを開けませんでした: " << filename << std::endl;
		return;
	}

	ofs << "PATTERNS=" << patterns.size() << "\n";

	for (size_t i = 0; i < patterns.size(); ++i) {
		ofs << "PATTERN" << i << "=";
		for (size_t j = 0; j < patterns[i].size(); ++j) {
			ofs << patterns[i][j];
			if (j < patterns[i].size() - 1) ofs << ",";
		}
		ofs << "\n";

		ofs << "COLORS" << i << "=";
		for (int c = 0; c < 3; ++c) {
			if (i < colorSets.size()) {
				ofs << (int)colorSets[i][c].r << "," << (int)colorSets[i][c].g << "," << (int)colorSets[i][c].b;
			}
			else {
				ofs << "255,255,255";
			}
			if (c < 2) ofs << ",";
		}
		ofs << "\n";
	}

	ofs << "CANVAS=" << canvas.size() << "," << (canvas.empty() ? 0 : canvas[0].size()) << "\n";
	for (const auto& row : canvas) {
		for (size_t x = 0; x < row.size(); ++x) {
			ofs << row[x];
			if (x < row.size() - 1) ofs << ",";
		}
		ofs << "\n";
	}

	ofs.close();
	std::cout << "テキスト形式で保存しました: " << filename << std::endl;
}