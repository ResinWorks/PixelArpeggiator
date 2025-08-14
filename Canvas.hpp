#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

// �O���錾
class CanvasView;

class Canvas {
private:
	int width, height;
	int tileSize;
	sf::Vector2f position;
	std::vector<std::vector<int>> tiles;

	// �p�t�H�[�}���X�œK���p
	bool isDirty = true;
	bool isInitialized = false;
	sf::RenderTexture renderTexture;

	// �O��̕`��ݒ���L�^
	bool lastShowGrid = false;
	float lastSpacing = 0.0f;
	float lastShrink = 0.0f;

	void initializeRenderTexture();
	void renderToTexture(const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid, float spacing, float shrink);

	// �^�C�������O���b�h�F�̐ݒ�i�V�K�ǉ��j
	sf::Color tileGridColor = sf::Color(128, 128, 128); // �f�t�H���g�F���ԃO���[
	bool useTileGridColor = true; // �^�C�������O���b�h�F���g�p���邩

public:
	Canvas(int width, int height, int tileSize, sf::Vector2f position)
		: width(width), height(height), tileSize(tileSize), position(position) {
		tiles.resize(height, std::vector<int>(width, -1));
	}

	// ===== �����̃��\�b�h�i�ύX�Ȃ��j =====
	void setPosition(const sf::Vector2f& pos) {
		if (position != pos) {
			position = pos;
			isDirty = true;
		}
	}

	void handleClick(const sf::Vector2i& mousePos, int selectedTileIndex);
	void setDirty(bool flag) { isDirty = flag; }

	void draw(sf::RenderWindow& window,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid = false,
		float spacing = 0.5f,
		float shrink = 1.0f);

	std::vector<std::vector<int>> getTileIndices() const {
		return tiles;
	}

	void setTileIndices(const std::vector<std::vector<int>>& newTiles) {
		if (newTiles.size() == height && newTiles[0].size() == width) {
			tiles = newTiles;
			isDirty = true;
		}
	}

	int getTileSize() const { return tileSize; }
	sf::Vector2f getPosition() const { return position; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }

	bool contains(const sf::Vector2i& mousePos) const {
		return sf::FloatRect(position.x, position.y, width * tileSize, height * tileSize).contains(static_cast<sf::Vector2f>(mousePos));
	}

	void eraseTile(const sf::Vector2i& position);
	void setTile(const sf::Vector2i& position, int tileIndex);

	// ===== CanvasView�Ή����\�b�h�i�錾�̂݁j =====
	void drawWithView(sf::RenderWindow& window,
		const CanvasView& view,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid = false,
		float spacing = 0.5f,
		float shrink = 1.0f);

	bool containsInView(const CanvasView& view, const sf::Vector2i& screenPos) const;
	void handleClickInView(const CanvasView& view, const sf::Vector2i& screenPos, int patternIndex);
	void eraseTileInView(const CanvasView& view, const sf::Vector2i& screenPos);

	struct PerformanceInfo {
		int totalTiles;
		int visibleTiles;
		int drawnTiles;
		float zoomLevel;
	};

	PerformanceInfo getPerformanceInfo(const CanvasView& view) const;

	sf::Vector2i screenToTileIndex(const CanvasView& view, const sf::Vector2i& screenPos) const;


	// ===== �V�������\�b�h�F�O���[�o���J���[�V�X�e���Ή� =====
	void drawWithViewAndGlobalColors(sf::RenderWindow& window,
		const CanvasView& view,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<int, 3>>& globalColorIndices,
		const std::array<sf::Color, 16>& globalColors,
		bool showGrid = false,
		float spacing = 0.5f,
		float shrink = 1.0f);

	void drawWithGlobalColors(sf::RenderWindow& window,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<int, 3>>& globalColorIndices,
		const std::array<sf::Color, 16>& globalColors,
		bool showGrid = false,
		float spacing = 0.5f,
		float shrink = 1.0f);

	bool exportToImageWithGlobalColors(const std::string& filename,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<int, 3>>& globalColorIndices,
		const std::array<sf::Color, 16>& globalColors,
		bool showGrid = false,
		float spacing = 0.5f,
		float shrink = 1.0f);


	/**
	 * �L�����o�X���摜�t�@�C���Ƃ��ďo��
	 * @param filename �o�̓t�@�C�����i�g���q���݁j
	 * @param patterns �p�^�[���f�[�^
	 * @param colorPalettes �J���[�p���b�g�f�[�^
	 * @param showGrid �O���b�h�\�����邩
	 * @param spacing �O���b�h�Ԋu
	 * @param shrink �^�C���k����
	 * @return �ۑ�������true
	 */
	bool exportToImage(const std::string& filename,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid = false,
		float spacing = 0.5f,
		float shrink = 1.0f);

	

	/**
	 * ���݂̃L�����o�X�T�C�Y���擾
	 * @return {��, ����}�̃y�A�i�s�N�Z���P�ʁj
	 */
	std::pair<int, int> getCanvasPixelSize() const {
		return { width * tileSize, height * tileSize };
	}

	/**
	 * �T�|�[�g����Ă���摜�`�����擾
	 * @return �T�|�[�g�`���̃��X�g
	 */
	static std::vector<std::string> getSupportedFormats() {
		return { "png", "jpg", "jpeg", "bmp", "tga" };
	}

	/**
	* �^�C�������O���b�h�F��ݒ�
	* @param color �V�����O���b�h�F
	*/
	void setTileGridColor(const sf::Color& color) {
		tileGridColor = color;
		isDirty = true; // �ĕ`��t���O
	}

	/**
	 * �^�C�������O���b�h�F���擾
	 * @return ���݂̃O���b�h�F
	 */
	sf::Color getTileGridColor() const {
		return tileGridColor;
	}

	/**
	 * �^�C�������O���b�h�F�̎g�p��Ԃ�ݒ�
	 * @param enabled true: �O���b�h�F���g�p, false: ����
	 */
	void setTileGridColorEnabled(bool enabled) {
		if (useTileGridColor != enabled) {
			useTileGridColor = enabled;
			isDirty = true;
		}
	}

	/**
	 * �^�C�������O���b�h�F�̎g�p��Ԃ��擾
	 */
	bool isTileGridColorEnabled() const {
		return useTileGridColor;
	}

private:

	/**
	* �摜�o�͗p�̓�������
	* @param outputTexture �o�͗pRenderTexture
	* @param patterns �p�^�[���f�[�^
	* @param colorPalettes �J���[�p���b�g�f�[�^
	* @param showGrid �O���b�h�\�����邩
	* @param spacing �O���b�h�Ԋu
	* @param shrink �^�C���k����
	* @param scale �X�P�[���{���i1.0 = ���T�C�Y�j
	*/

	
	void renderToOutputTexture(sf::RenderTexture& outputTexture,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid,
		float spacing,
		float shrink,
		float scale = 1.0f);
	

	/**
   * �X�V���ꂽ�`�揈���i�^�C�������O���b�h�F�Ή��j
   */
	/*
	void renderToTextureWithGridColor(const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid, float spacing, float shrink);
	*/

	// �V�����v���C�x�[�g���\�b�h�F�O���[�o���J���[�Ή�
	void renderToTextureWithGlobalColors(const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<int, 3>>& globalColorIndices,
		const std::array<sf::Color, 16>& globalColors,
		bool showGrid, float spacing, float shrink);

	void renderToOutputTextureWithGlobalColors(sf::RenderTexture& outputTexture,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<int, 3>>& globalColorIndices,
		const std::array<sf::Color, 16>& globalColors,
		bool showGrid, float spacing, float shrink, float scale = 1.0f);

};