#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "AppSettings.hpp"

// --- 型別名定義 ---
using PatternData = std::vector<int>;
using ColorSet = std::array<sf::Color, 3>;
using CanvasData = std::vector<std::vector<int>>;
using GlobalColorIndices = std::array<int, 3>;

// ファイル形式のバージョン定義
const int SAVE_FORMAT_VERSION_V1 = 1; // 旧形式（個別カラーセット）
const int SAVE_FORMAT_VERSION_V2 = 2; // 新形式（グローバルカラーシステム）



// --- 既存の関数（後方互換性のため維持） ---

// 旧形式でのセーブ関数
void saveProject(const std::string& filename,
	const std::vector<PatternData>& patterns,
	const std::vector<ColorSet>& colorSets,
	const CanvasData& canvas) {
	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs.is_open()) {
		std::cerr << "ファイルを開けませんでした: " << filename << std::endl;
		return;
	}

	// バージョン情報（V1形式）
	int version = SAVE_FORMAT_VERSION_V1;
	ofs.write(reinterpret_cast<const char*>(&version), sizeof(version));

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
	std::cout << "プロジェクトを保存しました（旧形式）: " << filename << std::endl;
}



/*
*/
// 旧形式でのロード関数（修正版：バージョンチェック対応）
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

		// バージョン情報を読み込み
		int version;
		ifs.read(reinterpret_cast<char*>(&version), sizeof(version));

		if (ifs.fail()) {
			std::cerr << "バージョン情報の読み込みエラー" << std::endl;
			return false;
		}

		if (version != SAVE_FORMAT_VERSION_V1) {
			std::cerr << "サポートされていないバージョン（旧形式ロード）: " << version << std::endl;
			return false;
		}

		// パターン数を読み込み
		size_t patternCount;
		ifs.read(reinterpret_cast<char*>(&patternCount), sizeof(patternCount));

		if (ifs.fail() || patternCount > 100) {
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

		if (ifs.fail() || canvasHeight > 1000 || canvasWidth > 1000) {
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

// テキスト形式（JSON風）でのセーブ関数（デバッグ用） - 既存のまま
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
// --- 新しいグローバルカラー対応セーブ関数 ---
void saveProjectWithGlobalColors(const std::string& filename,
	const std::vector<PatternData>& patterns,
	const std::vector<GlobalColorIndices>& globalColorIndices,
	const std::array<sf::Color, 16>& globalColorPalette,
	const CanvasData& canvas) {

	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs.is_open()) {
		std::cerr << "ファイルを開けませんでした: " << filename << std::endl;
		return;
	}

	// バージョン情報を保存
	int version = SAVE_FORMAT_VERSION_V2;
	ofs.write(reinterpret_cast<const char*>(&version), sizeof(version));

	// グローバルカラーパレット（16色）を保存
	for (int i = 0; i < 16; ++i) {
		sf::Uint8 r = globalColorPalette[i].r;
		sf::Uint8 g = globalColorPalette[i].g;
		sf::Uint8 b = globalColorPalette[i].b;
		ofs.write(reinterpret_cast<const char*>(&r), sizeof(r));
		ofs.write(reinterpret_cast<const char*>(&g), sizeof(g));
		ofs.write(reinterpret_cast<const char*>(&b), sizeof(b));
	}

	// パターン数を保存
	size_t patternCount = patterns.size();
	ofs.write(reinterpret_cast<const char*>(&patternCount), sizeof(patternCount));

	// 各パターンとグローバルカラーインデックスを保存
	for (size_t i = 0; i < patterns.size(); ++i) {
		// パターンデータ（9要素固定）
		for (int j = 0; j < 9; ++j) {
			int value = (j < patterns[i].size()) ? patterns[i][j] : 0;
			ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
		}

		// グローバルカラーインデックス（3要素）
		for (int c = 0; c < 3; ++c) {
			int globalIndex = (i < globalColorIndices.size()) ? globalColorIndices[i][c] : c;
			ofs.write(reinterpret_cast<const char*>(&globalIndex), sizeof(globalIndex));
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
	std::cout << "グローバルカラープロジェクトを保存しました: " << filename << std::endl;
}

// --- 新しいグローバルカラー対応ロード関数 ---
bool loadProjectWithGlobalColors(const std::string& filename,
	std::vector<PatternData>& patternsOut,
	std::vector<GlobalColorIndices>& globalColorIndicesOut,
	std::array<sf::Color, 16>& globalColorPaletteOut,
	CanvasData& canvasOut) {

	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs.is_open()) {
		std::cerr << "ファイルが開けません: " << filename << std::endl;
		return false;
	}

	try {
		// 結果をクリア
		patternsOut.clear();
		globalColorIndicesOut.clear();
		canvasOut.clear();

		// バージョン情報を読み込み
		int version;
		ifs.read(reinterpret_cast<char*>(&version), sizeof(version));

		if (ifs.fail()) {
			std::cerr << "バージョン情報の読み込みエラー" << std::endl;
			return false;
		}

		if (version != SAVE_FORMAT_VERSION_V2) {
			std::cerr << "サポートされていないバージョン: " << version << std::endl;
			return false;
		}

		std::cout << "バージョン " << version << " のファイルを読み込み中..." << std::endl;

		// グローバルカラーパレット（16色）を読み込み
		for (int i = 0; i < 16; ++i) {
			sf::Uint8 r, g, b;
			ifs.read(reinterpret_cast<char*>(&r), sizeof(r));
			ifs.read(reinterpret_cast<char*>(&g), sizeof(g));
			ifs.read(reinterpret_cast<char*>(&b), sizeof(b));
			if (ifs.fail()) {
				std::cerr << "グローバルカラー" << i << "の読み込みエラー" << std::endl;
				return false;
			}
			globalColorPaletteOut[i] = sf::Color(r, g, b);
		}

		// パターン数を読み込み
		size_t patternCount;
		ifs.read(reinterpret_cast<char*>(&patternCount), sizeof(patternCount));

		if (ifs.fail() || patternCount > 100) {
			std::cerr << "不正なパターン数: " << patternCount << std::endl;
			return false;
		}

		std::cout << "読み込むパターン数: " << patternCount << std::endl;

		// 各パターンとグローバルカラーインデックスを読み込み
		for (size_t i = 0; i < patternCount; ++i) {
			// パターンデータ（9要素）
			std::vector<int> pattern(9);
			for (int j = 0; j < 9; ++j) {
				ifs.read(reinterpret_cast<char*>(&pattern[j]), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "パターン" << i << "の読み込みエラー" << std::endl;
					return false;
				}

				if (pattern[j] == 3) {
					//	std::cerr << "透明色: パターン" << i << "、タイル "<<j << " のパターン値を-1に変換" << std::endl;
					pattern[j] = -1; // 透明色を-1に変換
				}
				// 透明色対応：パターンデータの妥当性チェック
				else if (pattern[j] < -1 || pattern[j] > 2) {

					std::cerr << "不正なパターン値: " << pattern[j] << " (パターン" << i << "のパターン値を-1に変換)" << std::endl;
					// 修正：範囲外の値を透明色に変換
					pattern[j] = -1;
				}
			}
			patternsOut.push_back(pattern);

			// グローバルカラーインデックス（3要素）
			GlobalColorIndices indices;
			for (int c = 0; c < 3; ++c) {
				ifs.read(reinterpret_cast<char*>(&indices[c]), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "グローバルカラーインデックス" << i << "-" << c << "の読み込みエラー" << std::endl;
					return false;
				}
				// 妥当性チェック
				if (indices[c] < 0 || indices[c] >= 16) {
					std::cerr << "不正なグローバルカラーインデックス: " << indices[c] << std::endl;
					indices[c] = std::min(std::max(0, indices[c]), 15); // 範囲内に修正
				}
			}
			globalColorIndicesOut.push_back(indices);
		}

		// キャンバスサイズを読み込み
		size_t canvasHeight, canvasWidth;
		ifs.read(reinterpret_cast<char*>(&canvasHeight), sizeof(canvasHeight));
		ifs.read(reinterpret_cast<char*>(&canvasWidth), sizeof(canvasWidth));

		if (ifs.fail() || canvasHeight > 1000 || canvasWidth > 1000) {
			std::cerr << "不正なキャンバスサイズ: " << canvasWidth << "x" << canvasHeight << std::endl;
			return false;
		}

		std::cout << "キャンバスサイズ: " << canvasWidth << "x" << canvasHeight << std::endl;


		/*

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



				// キャンバスデータの妥当性チェック（透明色対応）
				// キャンバス上のタイルインデックスは-1（空）または0以上の値
				if (canvasOut[y][x] < -1) {
					std::cerr << "不正なキャンバスタイルインデックス: " << canvasOut[y][x] << std::endl;
					canvasOut[y][x] = -1; // 空タイルに修正
				}
			}
		}
		*/

		// キャンバスデータを読み込み（可変サイズ対応）
		canvasOut.resize(AppSettings::canvasHeight); // 現在の設定サイズでリサイズ
		for (size_t y = 0; y < AppSettings::canvasHeight; ++y) {
			canvasOut[y].resize(AppSettings::canvasWidth, -1); // -1で初期化
		}

		// ファイルからデータを読み込み（重なる部分のみ）
		size_t readHeight = std::min(canvasHeight, static_cast<size_t>(AppSettings::canvasHeight));
		size_t readWidth = std::min(canvasWidth, static_cast<size_t>(AppSettings::canvasWidth));

		for (size_t y = 0; y < canvasHeight; ++y) {
			for (size_t x = 0; x < canvasWidth; ++x) {
				int tileValue;
				ifs.read(reinterpret_cast<char*>(&tileValue), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "キャンバス(" << x << "," << y << ")の読み込みエラー" << std::endl;
					return false;
				}

				// 現在のキャンバスサイズ内の場合のみ設定
				if (y < readHeight && x < readWidth) {
					canvasOut[y][x] = tileValue;
				}

				if (canvasOut[y][x] < -1) {
					std::cerr << "不正なキャンバスタイルインデックス: " << canvasOut[y][x] << std::endl;
					canvasOut[y][x] = -1; // 空タイルに修正
				}
			}
		}

		std::cout << "読み込み完了（透明色対応）: "
			<< patternsOut.size() << "パターン, "
			<< globalColorIndicesOut.size() << "グローバルカラーインデックス, "
			<< canvasOut.size() << "キャンバス行" << std::endl;

		return true;

	}
	catch (const std::exception& e) {
		std::cerr << "読み込み中にエラーが発生しました: " << e.what() << std::endl;
		return false;
	}
}


// --- 統合ロード関数（新旧形式自動判別） ---
bool loadProjectAuto(const std::string& filename,
	std::vector<PatternData>& patternsOut,
	std::vector<ColorSet>& colorSetsOut,
	std::vector<GlobalColorIndices>& globalColorIndicesOut,
	std::array<sf::Color, 16>& globalColorPaletteOut,
	CanvasData& canvasOut,
	bool& isGlobalColorFormat) {

	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs.is_open()) {
		std::cerr << "ファイルが開けません: " << filename << std::endl;
		return false;
	}

	// バージョン情報をチェック
	int version;
	ifs.read(reinterpret_cast<char*>(&version), sizeof(version));
	ifs.close();

	if (version == SAVE_FORMAT_VERSION_V2) {
		// 新形式（グローバルカラー）
		std::cout << "グローバルカラー形式のファイルを検出" << std::endl;
		isGlobalColorFormat = true;
		return loadProjectWithGlobalColors(filename, patternsOut, globalColorIndicesOut, globalColorPaletteOut, canvasOut);
	}
	else {
		// 旧形式（個別カラーセット）
		std::cout << "旧形式のファイルを検出、個別カラーセットとして読み込み" << std::endl;
		isGlobalColorFormat = false;
		return loadProject(filename, patternsOut, colorSetsOut, canvasOut);
	}
}

