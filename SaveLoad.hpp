#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <iostream>

// --- �^�ʖ���` ---
using PatternData = std::vector<int>;
using ColorSet = std::array<sf::Color, 3>;
using CanvasData = std::vector<std::vector<int>>;



// --- �V���v���ȃo�C�i���`���ł̃Z�[�u�֐� ---
void saveProject(const std::string& filename,
	const std::vector<PatternData>& patterns,
	const std::vector<ColorSet>& colorSets,
	const CanvasData& canvas) {
	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs.is_open()) {
		std::cerr << "�t�@�C�����J���܂���ł���: " << filename << std::endl;
		return;
	}

	// �p�^�[������ۑ�
	size_t patternCount = patterns.size();
	ofs.write(reinterpret_cast<const char*>(&patternCount), sizeof(patternCount));

	// �e�p�^�[���ƐF��ۑ�
	for (size_t i = 0; i < patterns.size(); ++i) {
		// �p�^�[���f�[�^�i9�v�f�Œ�j
		for (int j = 0; j < 9; ++j) {
			int value = (j < patterns[i].size()) ? patterns[i][j] : 0;
			ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
		}

		// �F�f�[�^�iRGB�e�F�j
		for (int c = 0; c < 3; ++c) {
			sf::Uint8 r = (i < colorSets.size()) ? colorSets[i][c].r : 255;
			sf::Uint8 g = (i < colorSets.size()) ? colorSets[i][c].g : 255;
			sf::Uint8 b = (i < colorSets.size()) ? colorSets[i][c].b : 255;
			ofs.write(reinterpret_cast<const char*>(&r), sizeof(r));
			ofs.write(reinterpret_cast<const char*>(&g), sizeof(g));
			ofs.write(reinterpret_cast<const char*>(&b), sizeof(b));
		}
	}

	// �L�����o�X�T�C�Y��ۑ�
	size_t canvasHeight = canvas.size();
	size_t canvasWidth = (canvasHeight > 0) ? canvas[0].size() : 0;
	ofs.write(reinterpret_cast<const char*>(&canvasHeight), sizeof(canvasHeight));
	ofs.write(reinterpret_cast<const char*>(&canvasWidth), sizeof(canvasWidth));

	// �L�����o�X�f�[�^��ۑ�
	for (const auto& row : canvas) {
		for (size_t x = 0; x < canvasWidth; ++x) {
			int value = (x < row.size()) ? row[x] : -1;
			ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
		}
	}

	ofs.close();
	std::cout << "�v���W�F�N�g��ۑ����܂���: " << filename << std::endl;
}

// --- �V���v���ȃo�C�i���`���ł̃��[�h�֐� ---
bool loadProject(const std::string& filename,
	std::vector<PatternData>& patternsOut,
	std::vector<ColorSet>& colorSetsOut,
	CanvasData& canvasOut) {
	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs.is_open()) {
		std::cerr << "�t�@�C�����J���܂���: " << filename << std::endl;
		return false;
	}

	try {
		// ���ʂ��N���A
		patternsOut.clear();
		colorSetsOut.clear();
		canvasOut.clear();

		// �p�^�[������ǂݍ���
		size_t patternCount;
		ifs.read(reinterpret_cast<char*>(&patternCount), sizeof(patternCount));

		if (ifs.fail() || patternCount > 100) { // �Ó����`�F�b�N
			std::cerr << "�s���ȃp�^�[����: " << patternCount << std::endl;
			return false;
		}

		std::cout << "�ǂݍ��ރp�^�[����: " << patternCount << std::endl;

		// �e�p�^�[���ƐF��ǂݍ���
		for (size_t i = 0; i < patternCount; ++i) {
			// �p�^�[���f�[�^�i9�v�f�j
			std::vector<int> pattern(9);
			for (int j = 0; j < 9; ++j) {
				ifs.read(reinterpret_cast<char*>(&pattern[j]), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "�p�^�[��" << i << "�̓ǂݍ��݃G���[" << std::endl;
					return false;
				}
			}
			patternsOut.push_back(pattern);

			// �F�f�[�^�i3�F�j
			std::array<sf::Color, 3> colors;
			for (int c = 0; c < 3; ++c) {
				sf::Uint8 r, g, b;
				ifs.read(reinterpret_cast<char*>(&r), sizeof(r));
				ifs.read(reinterpret_cast<char*>(&g), sizeof(g));
				ifs.read(reinterpret_cast<char*>(&b), sizeof(b));
				if (ifs.fail()) {
					std::cerr << "�F" << i << "-" << c << "�̓ǂݍ��݃G���[" << std::endl;
					return false;
				}
				colors[c] = sf::Color(r, g, b);
			}
			colorSetsOut.push_back(colors);
		}

		// �L�����o�X�T�C�Y��ǂݍ���
		size_t canvasHeight, canvasWidth;
		ifs.read(reinterpret_cast<char*>(&canvasHeight), sizeof(canvasHeight));
		ifs.read(reinterpret_cast<char*>(&canvasWidth), sizeof(canvasWidth));

		if (ifs.fail() || canvasHeight > 1000 || canvasWidth > 1000) { // �Ó����`�F�b�N
			std::cerr << "�s���ȃL�����o�X�T�C�Y: " << canvasWidth << "x" << canvasHeight << std::endl;
			return false;
		}

		std::cout << "�L�����o�X�T�C�Y: " << canvasWidth << "x" << canvasHeight << std::endl;

		// �L�����o�X�f�[�^��ǂݍ���
		canvasOut.resize(canvasHeight);
		for (size_t y = 0; y < canvasHeight; ++y) {
			canvasOut[y].resize(canvasWidth);
			for (size_t x = 0; x < canvasWidth; ++x) {
				ifs.read(reinterpret_cast<char*>(&canvasOut[y][x]), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "�L�����o�X(" << x << "," << y << ")�̓ǂݍ��݃G���[" << std::endl;
					return false;
				}
			}
		}

		std::cout << "�ǂݍ��݊���: "
			<< patternsOut.size() << "�p�^�[��, "
			<< colorSetsOut.size() << "�F�Z�b�g, "
			<< canvasOut.size() << "�L�����o�X�s" << std::endl;

		return true;

	}
	catch (const std::exception& e) {
		std::cerr << "�ǂݍ��ݒ��ɃG���[���������܂���: " << e.what() << std::endl;
		return false;
	}
}

// --- �e�L�X�g�`���iJSON���j�ł̃Z�[�u�֐��i�f�o�b�O�p�j ---
void saveProjectText(const std::string& filename,
	const std::vector<PatternData>& patterns,
	const std::vector<ColorSet>& colorSets,
	const CanvasData& canvas) {
	std::ofstream ofs(filename);
	if (!ofs.is_open()) {
		std::cerr << "�t�@�C�����J���܂���ł���: " << filename << std::endl;
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
	std::cout << "�e�L�X�g�`���ŕۑ����܂���: " << filename << std::endl;
}