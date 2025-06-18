#pragma once
//===== DrawingTools.hpp =====
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <set>

// 前方宣言
class Canvas;
class CanvasView;

/**
 * 描画ツールの基底クラス
 * すべての描画ツール（ブラシ、消しゴム、直線等）の共通インターフェース
 */
class DrawingTool {
public:
	virtual ~DrawingTool() = default;

	/**
	 * 描画開始時の処理
	 * @param startPos 開始位置（スクリーン座標）
	 * @param canvas 描画対象キャンバス
	 * @param view CanvasViewインスタンス
	 * @param patternIndex 選択されたパターンインデックス
	 * @param brushSize ブラシサイズ
	 */
	virtual void onDrawStart(const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) = 0;

	/**
	 * 描画中の処理（マウスドラッグ時）
	 * @param currentPos 現在位置（スクリーン座標）
	 * @param lastPos 前回位置（スクリーン座標）
	 * @param canvas 描画対象キャンバス
	 * @param view CanvasViewインスタンス
	 * @param patternIndex 選択されたパターンインデックス
	 * @param brushSize ブラシサイズ
	 */
	virtual void onDrawContinue(const sf::Vector2i& currentPos,
		const sf::Vector2i& lastPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) = 0;

	/**
	 * 描画終了時の処理
	 * @param endPos 終了位置（スクリーン座標）
	 * @param startPos 開始位置（スクリーン座標）
	 * @param canvas 描画対象キャンバス
	 * @param view CanvasViewインスタンス
	 * @param patternIndex 選択されたパターンインデックス
	 * @param brushSize ブラシサイズ
	 */
	virtual void onDrawEnd(const sf::Vector2i& endPos,
		const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) = 0;

	/**
	 * プレビュー描画（実際には描画せず、画面上にプレビュー表示）
	 * @param window 描画ウィンドウ
	 * @param startPos 開始位置（スクリーン座標）
	 * @param currentPos 現在位置（スクリーン座標）
	 * @param view CanvasViewインスタンス
	 * @param brushSize ブラシサイズ
	 */
	virtual void drawPreview(sf::RenderWindow& window,
		const sf::Vector2i& startPos,
		const sf::Vector2i& currentPos,
		const CanvasView& view,
		int brushSize) const = 0;

	/**
	 * カーソル描画
	 * @param window 描画ウィンドウ
	 * @param mousePos マウス位置（スクリーン座標）
	 * @param view CanvasViewインスタンス
	 * @param brushSize ブラシサイズ
	 * @param tileSize タイルサイズ
	 */
	virtual void drawCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize) const = 0;

	/**
	 * ツール名を取得
	 */
	virtual std::string getToolName() const = 0;

	/**
	 * このツールが連続描画をサポートするか
	 * （ブラシ：true、直線：false）
	 */
	virtual bool supportsContinuousDrawing() const = 0;

protected:
	/**
	 * ブラシを指定位置に適用するヘルパー関数
	 * @param position 位置（スクリーン座標）
	 * @param brushSize ブラシサイズ
	 * @param canvas 描画対象キャンバス
	 * @param view CanvasViewインスタンス
	 * @param patternIndex パターンインデックス（-1で消去）
	 */
	void applyBrushAt(const sf::Vector2i& position,
		int brushSize,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex) const;

	/**
	 * 線に沿ってブラシを適用するヘルパー関数
	 * @param startPos 開始位置（スクリーン座標）
	 * @param endPos 終了位置（スクリーン座標）
	 * @param brushSize ブラシサイズ
	 * @param canvas 描画対象キャンバス
	 * @param view CanvasViewインスタンス
	 * @param patternIndex パターンインデックス（-1で消去）
	 */
	void applyBrushAlongLine(const sf::Vector2i& startPos,
		const sf::Vector2i& endPos,
		int brushSize,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex) const;

	/**
	 * 基本的なブラシカーソルを描画するヘルパー関数
	 * @param window 描画ウィンドウ
	 * @param mousePos マウス位置
	 * @param view CanvasViewインスタンス
	 * @param brushSize ブラシサイズ
	 * @param tileSize タイルサイズ
	 * @param color カーソル色
	 */
	void drawBasicCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize,
		const sf::Color& color) const;
};

// ===== 具象ツールクラス =====

/**
 * ブラシツール
 * 従来のブラシ機能を実装
 */
class BrushTool : public DrawingTool {
public:
	void onDrawStart(const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawContinue(const sf::Vector2i& currentPos,
		const sf::Vector2i& lastPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawEnd(const sf::Vector2i& endPos,
		const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void drawPreview(sf::RenderWindow& window,
		const sf::Vector2i& startPos,
		const sf::Vector2i& currentPos,
		const CanvasView& view,
		int brushSize) const override;

	void drawCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize) const override;

	std::string getToolName() const override { return "Brush"; }
	bool supportsContinuousDrawing() const override { return true; }
};

/**
 * 消しゴムツール
 * 従来の消しゴム機能を実装
 */
class EraserTool : public DrawingTool {
public:
	void onDrawStart(const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawContinue(const sf::Vector2i& currentPos,
		const sf::Vector2i& lastPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawEnd(const sf::Vector2i& endPos,
		const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void drawPreview(sf::RenderWindow& window,
		const sf::Vector2i& startPos,
		const sf::Vector2i& currentPos,
		const CanvasView& view,
		int brushSize) const override;

	void drawCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize) const override;

	std::string getToolName() const override { return "Eraser"; }
	bool supportsContinuousDrawing() const override { return true; }
};

/**
 * 直線ツール
 * クリック＆ドラッグで直線を描画
 */
class LineTool : public DrawingTool {
public:
	void onDrawStart(const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawContinue(const sf::Vector2i& currentPos,
		const sf::Vector2i& lastPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawEnd(const sf::Vector2i& endPos,
		const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void drawPreview(sf::RenderWindow& window,
		const sf::Vector2i& startPos,
		const sf::Vector2i& currentPos,
		const CanvasView& view,
		int brushSize) const override;

	void drawCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize) const override;

	std::string getToolName() const override { return "Line"; }
	bool supportsContinuousDrawing() const override { return false; }
};

/**
 * 円ツール
 * クリック＆ドラッグで円を描画（中心から半径を指定）
 */
 /*
 */
class CircleTool : public DrawingTool {
public:
	void onDrawStart(const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawContinue(const sf::Vector2i& currentPos,
		const sf::Vector2i& lastPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawEnd(const sf::Vector2i& endPos,
		const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void drawPreview(sf::RenderWindow& window,
		const sf::Vector2i& startPos,
		const sf::Vector2i& currentPos,
		const CanvasView& view,
		int brushSize) const override;

	void drawCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize) const override;

	std::string getToolName() const override { return "Circle"; }
	bool supportsContinuousDrawing() const override { return false; }

private:
	/**
	 * 円周上の点を計算してブラシを適用
	 * @param centerPos 中心位置（スクリーン座標）
	 * @param radius 半径（スクリーン座標）
	 * @param brushSize ブラシサイズ
	 * @param canvas 描画対象キャンバス
	 * @param view CanvasViewインスタンス
	 * @param patternIndex パターンインデックス
	 */
	void drawCircle(const sf::Vector2i& centerPos,
		float radius,
		int brushSize,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex) const;
};


/**
 * 楕円ツール（円ツールを置き換え）
 * クリック＆ドラッグで楕円を描画（対角線で楕円を定義）
 */
class EllipseTool : public DrawingTool {
public:
	void onDrawStart(const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawContinue(const sf::Vector2i& currentPos,
		const sf::Vector2i& lastPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void onDrawEnd(const sf::Vector2i& endPos,
		const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) override;

	void drawPreview(sf::RenderWindow& window,
		const sf::Vector2i& startPos,
		const sf::Vector2i& currentPos,
		const CanvasView& view,
		int brushSize) const override;

	void drawCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize) const override;

	std::string getToolName() const override { return "Ellipse"; }
	bool supportsContinuousDrawing() const override { return false; }

private:
	/**
	 * 楕円の境界線を描画
	 * @param topLeft 楕円を囲む矩形の左上角（スクリーン座標）
	 * @param bottomRight 楕円を囲む矩形の右下角（スクリーン座標）
	 * @param brushSize ブラシサイズ
	 * @param canvas 描画対象キャンバス
	 * @param view CanvasViewインスタンス
	 * @param patternIndex パターンインデックス
	 */
	void drawEllipse(const sf::Vector2i& topLeft,
		const sf::Vector2i& bottomRight,
		int brushSize,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex) const;

	/**
	 * 楕円パラメータの計算
	 * @param topLeft 左上角
	 * @param bottomRight 右下角
	 * @return {centerX, centerY, radiusX, radiusY}
	 */
	struct EllipseParams {
		float centerX, centerY;
		float radiusX, radiusY;
	};

	EllipseParams calculateEllipseParams(const sf::Vector2i& topLeft,
		const sf::Vector2i& bottomRight) const;
};


/**
 * ツール管理クラス
 * 各ツールの生成・切り替えを管理
 */
class ToolManager {
private:
	std::unique_ptr<DrawingTool> currentTool;

public:
	enum class ToolType {
		BRUSH,
		ERASER,
		LINE,
		CIRCLE,  // 新規追加
		 ELLIPSE, // 新規追加
		LARGE_TILE  // 新規追加
	};

	ToolManager();

	/**
	 * ツールを切り替え
	 * @param toolType 切り替え先のツールタイプ
	 */
	void setTool(ToolType toolType);

	/**
	 * 現在のツールを取得
	 */
	DrawingTool* getCurrentTool() const { return currentTool.get(); }

	/**
	 * 現在のツールタイプを取得
	 */
	ToolType getCurrentToolType() const;

	/**
	 * ツール名を取得
	 */
	std::string getCurrentToolName() const;

private:
	ToolType currentToolType = ToolType::BRUSH;
};