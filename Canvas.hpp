#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

// 前方宣言
class CanvasView;

class Canvas {
private:
	int width, height;
	int tileSize;
	sf::Vector2f position;
	std::vector<std::vector<int>> tiles;

	// パフォーマンス最適化用
	bool isDirty = true;
	bool isInitialized = false;
	sf::RenderTexture renderTexture;

	// 前回の描画設定を記録
	bool lastShowGrid = false;
	float lastSpacing = 0.0f;
	float lastShrink = 0.0f;

	void initializeRenderTexture();
	void renderToTexture(const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid, float spacing, float shrink);

	// タイル内部グリッド色の設定（新規追加）
	sf::Color tileGridColor = sf::Color(128, 128, 128); // デフォルト：中間グレー
	bool useTileGridColor = true; // タイル内部グリッド色を使用するか

public:
	Canvas(int width, int height, int tileSize, sf::Vector2f position)
		: width(width), height(height), tileSize(tileSize), position(position) {
		tiles.resize(height, std::vector<int>(width, -1));
	}

	// ===== 既存のメソッド（変更なし） =====
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

	// ===== CanvasView対応メソッド（宣言のみ） =====
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


	// ===== 新しいメソッド：グローバルカラーシステム対応 =====
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
	 * キャンバスを画像ファイルとして出力
	 * @param filename 出力ファイル名（拡張子込み）
	 * @param patterns パターンデータ
	 * @param colorPalettes カラーパレットデータ
	 * @param showGrid グリッド表示するか
	 * @param spacing グリッド間隔
	 * @param shrink タイル縮小率
	 * @return 保存成功時true
	 */
	bool exportToImage(const std::string& filename,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid = false,
		float spacing = 0.5f,
		float shrink = 1.0f);

	

	/**
	 * 現在のキャンバスサイズを取得
	 * @return {幅, 高さ}のペア（ピクセル単位）
	 */
	std::pair<int, int> getCanvasPixelSize() const {
		return { width * tileSize, height * tileSize };
	}

	/**
	 * サポートされている画像形式を取得
	 * @return サポート形式のリスト
	 */
	static std::vector<std::string> getSupportedFormats() {
		return { "png", "jpg", "jpeg", "bmp", "tga" };
	}

	/**
	* タイル内部グリッド色を設定
	* @param color 新しいグリッド色
	*/
	void setTileGridColor(const sf::Color& color) {
		tileGridColor = color;
		isDirty = true; // 再描画フラグ
	}

	/**
	 * タイル内部グリッド色を取得
	 * @return 現在のグリッド色
	 */
	sf::Color getTileGridColor() const {
		return tileGridColor;
	}

	/**
	 * タイル内部グリッド色の使用状態を設定
	 * @param enabled true: グリッド色を使用, false: 透明
	 */
	void setTileGridColorEnabled(bool enabled) {
		if (useTileGridColor != enabled) {
			useTileGridColor = enabled;
			isDirty = true;
		}
	}

	/**
	 * タイル内部グリッド色の使用状態を取得
	 */
	bool isTileGridColorEnabled() const {
		return useTileGridColor;
	}

private:

	/**
	* 画像出力用の内部処理
	* @param outputTexture 出力用RenderTexture
	* @param patterns パターンデータ
	* @param colorPalettes カラーパレットデータ
	* @param showGrid グリッド表示するか
	* @param spacing グリッド間隔
	* @param shrink タイル縮小率
	* @param scale スケール倍率（1.0 = 元サイズ）
	*/

	
	void renderToOutputTexture(sf::RenderTexture& outputTexture,
		const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid,
		float spacing,
		float shrink,
		float scale = 1.0f);
	

	/**
   * 更新された描画処理（タイル内部グリッド色対応）
   */
	/*
	void renderToTextureWithGridColor(const std::vector<std::vector<int>>& patterns,
		const std::vector<std::array<sf::Color, 3>>& colorPalettes,
		bool showGrid, float spacing, float shrink);
	*/

	// 新しいプライベートメソッド：グローバルカラー対応
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