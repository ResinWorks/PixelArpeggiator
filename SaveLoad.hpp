#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "AppSettings.hpp"

// --- �^�ʖ���` ---
using PatternData = std::vector<int>;
using ColorSet = std::array<sf::Color, 3>;
using CanvasData = std::vector<std::vector<int>>;
using GlobalColorIndices = std::array<int, 3>;

// �t�@�C���`���̃o�[�W������`
const int SAVE_FORMAT_VERSION_V1 = 1; // ���`���i�ʃJ���[�Z�b�g�j
const int SAVE_FORMAT_VERSION_V2 = 2; // �V�`���i�O���[�o���J���[�V�X�e���j



// --- �����̊֐��i����݊����̂��߈ێ��j ---

// ���`���ł̃Z�[�u�֐�
void saveProject(const std::string& filename,
	const std::vector<PatternData>& patterns,
	const std::vector<ColorSet>& colorSets,
	const CanvasData& canvas) {
	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs.is_open()) {
		std::cerr << "�t�@�C�����J���܂���ł���: " << filename << std::endl;
		return;
	}

	// �o�[�W�������iV1�`���j
	int version = SAVE_FORMAT_VERSION_V1;
	ofs.write(reinterpret_cast<const char*>(&version), sizeof(version));

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
	std::cout << "�v���W�F�N�g��ۑ����܂����i���`���j: " << filename << std::endl;
}



/*
*/
// ���`���ł̃��[�h�֐��i�C���ŁF�o�[�W�����`�F�b�N�Ή��j
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

		// �o�[�W��������ǂݍ���
		int version;
		ifs.read(reinterpret_cast<char*>(&version), sizeof(version));

		if (ifs.fail()) {
			std::cerr << "�o�[�W�������̓ǂݍ��݃G���[" << std::endl;
			return false;
		}

		if (version != SAVE_FORMAT_VERSION_V1) {
			std::cerr << "�T�|�[�g����Ă��Ȃ��o�[�W�����i���`�����[�h�j: " << version << std::endl;
			return false;
		}

		// �p�^�[������ǂݍ���
		size_t patternCount;
		ifs.read(reinterpret_cast<char*>(&patternCount), sizeof(patternCount));

		if (ifs.fail() || patternCount > 100) {
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

		if (ifs.fail() || canvasHeight > 1000 || canvasWidth > 1000) {
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

// �e�L�X�g�`���iJSON���j�ł̃Z�[�u�֐��i�f�o�b�O�p�j - �����̂܂�
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
// --- �V�����O���[�o���J���[�Ή��Z�[�u�֐� ---
void saveProjectWithGlobalColors(const std::string& filename,
	const std::vector<PatternData>& patterns,
	const std::vector<GlobalColorIndices>& globalColorIndices,
	const std::array<sf::Color, 16>& globalColorPalette,
	const CanvasData& canvas) {

	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs.is_open()) {
		std::cerr << "�t�@�C�����J���܂���ł���: " << filename << std::endl;
		return;
	}

	// �o�[�W��������ۑ�
	int version = SAVE_FORMAT_VERSION_V2;
	ofs.write(reinterpret_cast<const char*>(&version), sizeof(version));

	// �O���[�o���J���[�p���b�g�i16�F�j��ۑ�
	for (int i = 0; i < 16; ++i) {
		sf::Uint8 r = globalColorPalette[i].r;
		sf::Uint8 g = globalColorPalette[i].g;
		sf::Uint8 b = globalColorPalette[i].b;
		ofs.write(reinterpret_cast<const char*>(&r), sizeof(r));
		ofs.write(reinterpret_cast<const char*>(&g), sizeof(g));
		ofs.write(reinterpret_cast<const char*>(&b), sizeof(b));
	}

	// �p�^�[������ۑ�
	size_t patternCount = patterns.size();
	ofs.write(reinterpret_cast<const char*>(&patternCount), sizeof(patternCount));

	// �e�p�^�[���ƃO���[�o���J���[�C���f�b�N�X��ۑ�
	for (size_t i = 0; i < patterns.size(); ++i) {
		// �p�^�[���f�[�^�i9�v�f�Œ�j
		for (int j = 0; j < 9; ++j) {
			int value = (j < patterns[i].size()) ? patterns[i][j] : 0;
			ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
		}

		// �O���[�o���J���[�C���f�b�N�X�i3�v�f�j
		for (int c = 0; c < 3; ++c) {
			int globalIndex = (i < globalColorIndices.size()) ? globalColorIndices[i][c] : c;
			ofs.write(reinterpret_cast<const char*>(&globalIndex), sizeof(globalIndex));
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
	std::cout << "�O���[�o���J���[�v���W�F�N�g��ۑ����܂���: " << filename << std::endl;
}

// --- �V�����O���[�o���J���[�Ή����[�h�֐� ---
bool loadProjectWithGlobalColors(const std::string& filename,
	std::vector<PatternData>& patternsOut,
	std::vector<GlobalColorIndices>& globalColorIndicesOut,
	std::array<sf::Color, 16>& globalColorPaletteOut,
	CanvasData& canvasOut) {

	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs.is_open()) {
		std::cerr << "�t�@�C�����J���܂���: " << filename << std::endl;
		return false;
	}

	try {
		// ���ʂ��N���A
		patternsOut.clear();
		globalColorIndicesOut.clear();
		canvasOut.clear();

		// �o�[�W��������ǂݍ���
		int version;
		ifs.read(reinterpret_cast<char*>(&version), sizeof(version));

		if (ifs.fail()) {
			std::cerr << "�o�[�W�������̓ǂݍ��݃G���[" << std::endl;
			return false;
		}

		if (version != SAVE_FORMAT_VERSION_V2) {
			std::cerr << "�T�|�[�g����Ă��Ȃ��o�[�W����: " << version << std::endl;
			return false;
		}

		std::cout << "�o�[�W���� " << version << " �̃t�@�C����ǂݍ��ݒ�..." << std::endl;

		// �O���[�o���J���[�p���b�g�i16�F�j��ǂݍ���
		for (int i = 0; i < 16; ++i) {
			sf::Uint8 r, g, b;
			ifs.read(reinterpret_cast<char*>(&r), sizeof(r));
			ifs.read(reinterpret_cast<char*>(&g), sizeof(g));
			ifs.read(reinterpret_cast<char*>(&b), sizeof(b));
			if (ifs.fail()) {
				std::cerr << "�O���[�o���J���[" << i << "�̓ǂݍ��݃G���[" << std::endl;
				return false;
			}
			globalColorPaletteOut[i] = sf::Color(r, g, b);
		}

		// �p�^�[������ǂݍ���
		size_t patternCount;
		ifs.read(reinterpret_cast<char*>(&patternCount), sizeof(patternCount));

		if (ifs.fail() || patternCount > 100) {
			std::cerr << "�s���ȃp�^�[����: " << patternCount << std::endl;
			return false;
		}

		std::cout << "�ǂݍ��ރp�^�[����: " << patternCount << std::endl;

		// �e�p�^�[���ƃO���[�o���J���[�C���f�b�N�X��ǂݍ���
		for (size_t i = 0; i < patternCount; ++i) {
			// �p�^�[���f�[�^�i9�v�f�j
			std::vector<int> pattern(9);
			for (int j = 0; j < 9; ++j) {
				ifs.read(reinterpret_cast<char*>(&pattern[j]), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "�p�^�[��" << i << "�̓ǂݍ��݃G���[" << std::endl;
					return false;
				}

				if (pattern[j] == 3) {
					//	std::cerr << "�����F: �p�^�[��" << i << "�A�^�C�� "<<j << " �̃p�^�[���l��-1�ɕϊ�" << std::endl;
					pattern[j] = -1; // �����F��-1�ɕϊ�
				}
				// �����F�Ή��F�p�^�[���f�[�^�̑Ó����`�F�b�N
				else if (pattern[j] < -1 || pattern[j] > 2) {

					std::cerr << "�s���ȃp�^�[���l: " << pattern[j] << " (�p�^�[��" << i << "�̃p�^�[���l��-1�ɕϊ�)" << std::endl;
					// �C���F�͈͊O�̒l�𓧖��F�ɕϊ�
					pattern[j] = -1;
				}
			}
			patternsOut.push_back(pattern);

			// �O���[�o���J���[�C���f�b�N�X�i3�v�f�j
			GlobalColorIndices indices;
			for (int c = 0; c < 3; ++c) {
				ifs.read(reinterpret_cast<char*>(&indices[c]), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "�O���[�o���J���[�C���f�b�N�X" << i << "-" << c << "�̓ǂݍ��݃G���[" << std::endl;
					return false;
				}
				// �Ó����`�F�b�N
				if (indices[c] < 0 || indices[c] >= 16) {
					std::cerr << "�s���ȃO���[�o���J���[�C���f�b�N�X: " << indices[c] << std::endl;
					indices[c] = std::min(std::max(0, indices[c]), 15); // �͈͓��ɏC��
				}
			}
			globalColorIndicesOut.push_back(indices);
		}

		// �L�����o�X�T�C�Y��ǂݍ���
		size_t canvasHeight, canvasWidth;
		ifs.read(reinterpret_cast<char*>(&canvasHeight), sizeof(canvasHeight));
		ifs.read(reinterpret_cast<char*>(&canvasWidth), sizeof(canvasWidth));

		if (ifs.fail() || canvasHeight > 1000 || canvasWidth > 1000) {
			std::cerr << "�s���ȃL�����o�X�T�C�Y: " << canvasWidth << "x" << canvasHeight << std::endl;
			return false;
		}

		std::cout << "�L�����o�X�T�C�Y: " << canvasWidth << "x" << canvasHeight << std::endl;


		/*

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



				// �L�����o�X�f�[�^�̑Ó����`�F�b�N�i�����F�Ή��j
				// �L�����o�X��̃^�C���C���f�b�N�X��-1�i��j�܂���0�ȏ�̒l
				if (canvasOut[y][x] < -1) {
					std::cerr << "�s���ȃL�����o�X�^�C���C���f�b�N�X: " << canvasOut[y][x] << std::endl;
					canvasOut[y][x] = -1; // ��^�C���ɏC��
				}
			}
		}
		*/

		// �L�����o�X�f�[�^��ǂݍ��݁i�σT�C�Y�Ή��j
		canvasOut.resize(AppSettings::canvasHeight); // ���݂̐ݒ�T�C�Y�Ń��T�C�Y
		for (size_t y = 0; y < AppSettings::canvasHeight; ++y) {
			canvasOut[y].resize(AppSettings::canvasWidth, -1); // -1�ŏ�����
		}

		// �t�@�C������f�[�^��ǂݍ��݁i�d�Ȃ镔���̂݁j
		size_t readHeight = std::min(canvasHeight, static_cast<size_t>(AppSettings::canvasHeight));
		size_t readWidth = std::min(canvasWidth, static_cast<size_t>(AppSettings::canvasWidth));

		for (size_t y = 0; y < canvasHeight; ++y) {
			for (size_t x = 0; x < canvasWidth; ++x) {
				int tileValue;
				ifs.read(reinterpret_cast<char*>(&tileValue), sizeof(int));
				if (ifs.fail()) {
					std::cerr << "�L�����o�X(" << x << "," << y << ")�̓ǂݍ��݃G���[" << std::endl;
					return false;
				}

				// ���݂̃L�����o�X�T�C�Y���̏ꍇ�̂ݐݒ�
				if (y < readHeight && x < readWidth) {
					canvasOut[y][x] = tileValue;
				}

				if (canvasOut[y][x] < -1) {
					std::cerr << "�s���ȃL�����o�X�^�C���C���f�b�N�X: " << canvasOut[y][x] << std::endl;
					canvasOut[y][x] = -1; // ��^�C���ɏC��
				}
			}
		}

		std::cout << "�ǂݍ��݊����i�����F�Ή��j: "
			<< patternsOut.size() << "�p�^�[��, "
			<< globalColorIndicesOut.size() << "�O���[�o���J���[�C���f�b�N�X, "
			<< canvasOut.size() << "�L�����o�X�s" << std::endl;

		return true;

	}
	catch (const std::exception& e) {
		std::cerr << "�ǂݍ��ݒ��ɃG���[���������܂���: " << e.what() << std::endl;
		return false;
	}
}


// --- �������[�h�֐��i�V���`���������ʁj ---
bool loadProjectAuto(const std::string& filename,
	std::vector<PatternData>& patternsOut,
	std::vector<ColorSet>& colorSetsOut,
	std::vector<GlobalColorIndices>& globalColorIndicesOut,
	std::array<sf::Color, 16>& globalColorPaletteOut,
	CanvasData& canvasOut,
	bool& isGlobalColorFormat) {

	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs.is_open()) {
		std::cerr << "�t�@�C�����J���܂���: " << filename << std::endl;
		return false;
	}

	// �o�[�W���������`�F�b�N
	int version;
	ifs.read(reinterpret_cast<char*>(&version), sizeof(version));
	ifs.close();

	if (version == SAVE_FORMAT_VERSION_V2) {
		// �V�`���i�O���[�o���J���[�j
		std::cout << "�O���[�o���J���[�`���̃t�@�C�������o" << std::endl;
		isGlobalColorFormat = true;
		return loadProjectWithGlobalColors(filename, patternsOut, globalColorIndicesOut, globalColorPaletteOut, canvasOut);
	}
	else {
		// ���`���i�ʃJ���[�Z�b�g�j
		std::cout << "���`���̃t�@�C�������o�A�ʃJ���[�Z�b�g�Ƃ��ēǂݍ���" << std::endl;
		isGlobalColorFormat = false;
		return loadProject(filename, patternsOut, colorSetsOut, canvasOut);
	}
}

