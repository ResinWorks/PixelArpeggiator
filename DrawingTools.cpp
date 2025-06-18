//===== DrawingTools.cpp =====
#include "DrawingTools.hpp"
#include "Canvas.hpp"
#include "CanvasView.hpp"
#include <cmath>
#include <algorithm>
#include "LargeTileSystem.hpp"

// ===== DrawingTool 基底クラスのヘルパー実装 =====

/**
 * ブラシを指定位置に適用するヘルパー関数
 * 複数タイルに対応したブラシサイズでの描画
 */
void DrawingTool::applyBrushAt(const sf::Vector2i& position,
	int brushSize,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex) const {

	std::set<std::pair<int, int>> processedPositions;
	int start = -(brushSize - 1) / 2;  // ブラシ範囲の開始オフセット
	int end = brushSize / 2;           // ブラシ範囲の終了オフセット

	// ブラシサイズ分の範囲でタイルを処理
	for (int dy = start; dy <= end; ++dy) {
		for (int dx = start; dx <= end; ++dx) {
			// ブラシの各点でのスクリーン座標を計算
			sf::Vector2i targetScreen(
				position.x + dx * static_cast<int>(canvas.getTileSize() * view.getZoom()),
				position.y + dy * static_cast<int>(canvas.getTileSize() * view.getZoom())
			);

			// キャンバス内かチェック
			if (canvas.containsInView(view, targetScreen)) {
				// タイルインデックスを取得して重複処理を避ける
				sf::Vector2i tileIndex = canvas.screenToTileIndex(view, targetScreen);

				std::pair<int, int> tilePos = { tileIndex.x, tileIndex.y };
				if (processedPositions.find(tilePos) == processedPositions.end()) {
					processedPositions.insert(tilePos);

					// パターンインデックスに応じて描画または消去
					if (patternIndex >= 0) {
						canvas.handleClickInView(view, targetScreen, patternIndex);
					}
					else {
						canvas.eraseTileInView(view, targetScreen);
					}
				}
			}
		}
	}
}

/**
 * 線に沿ってブラシを適用するヘルパー関数
 * 開始点から終了点までの直線上にブラシを適用
 */
void DrawingTool::applyBrushAlongLine(const sf::Vector2i& startPos,
	const sf::Vector2i& endPos,
	int brushSize,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex) const {

	// 線分の長さを計算
	int dx = std::abs(endPos.x - startPos.x);
	int dy = std::abs(endPos.y - startPos.y);
	int steps = std::max(dx, dy);
	if (steps == 0) steps = 1;

	// 線分を等間隔でサンプリングしてブラシを適用
	for (int i = 0; i <= steps; ++i) {
		float t = static_cast<float>(i) / steps;  // 補間パラメータ（0.0〜1.0）

		// 線形補間で現在の位置を計算
		int ix = static_cast<int>(startPos.x + t * (endPos.x - startPos.x));
		int iy = static_cast<int>(startPos.y + t * (endPos.y - startPos.y));

		applyBrushAt(sf::Vector2i(ix, iy), brushSize, canvas, view, patternIndex);
	}
}

/**
 * 基本的なブラシカーソルを描画するヘルパー関数
 * ズームレベルに応じたサイズ調整付き
 */
void DrawingTool::drawBasicCursor(sf::RenderWindow& window,
	const sf::Vector2i& mousePos,
	const CanvasView& view,
	int brushSize,
	int tileSize,
	const sf::Color& color) const {

	float zoom = view.getZoom();
	float cursorRadius = (brushSize * tileSize * zoom) / 2.0f;

	// カーソル円を描画
	sf::CircleShape cursor(cursorRadius);
	cursor.setOrigin(cursorRadius, cursorRadius);
	cursor.setPosition(static_cast<sf::Vector2f>(mousePos));
	cursor.setFillColor(color);
	cursor.setOutlineThickness(1.0f);
	cursor.setOutlineColor(sf::Color(255, 255, 255, 150));

	window.draw(cursor);

	// 高ズーム時は中心点を表示
	if (zoom > 2.0f) {
		sf::CircleShape centerDot(2.0f);
		centerDot.setOrigin(2.0f, 2.0f);
		centerDot.setPosition(static_cast<sf::Vector2f>(mousePos));
		centerDot.setFillColor(sf::Color::White);
		window.draw(centerDot);
	}
}

// ===== BrushTool 実装 =====

/**
 * ブラシツール：描画開始
 * 開始位置に即座にブラシを適用
 */
void BrushTool::onDrawStart(const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 開始位置に即座にブラシを適用
	applyBrushAt(startPos, brushSize, canvas, view, patternIndex);
}

/**
 * ブラシツール：描画継続
 * 前回位置から現在位置まで線に沿ってブラシを適用
 */
void BrushTool::onDrawContinue(const sf::Vector2i& currentPos,
	const sf::Vector2i& lastPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 前回位置から現在位置まで線に沿って描画
	applyBrushAlongLine(lastPos, currentPos, brushSize, canvas, view, patternIndex);
}

/**
 * ブラシツール：描画終了
 * 連続描画ツールなので特別な処理は不要
 */
void BrushTool::onDrawEnd(const sf::Vector2i& endPos,
	const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// ブラシツールは連続描画なので特別な終了処理は不要
}

/**
 * ブラシツール：プレビュー描画
 * ブラシは連続描画なのでプレビューは表示しない
 */
void BrushTool::drawPreview(sf::RenderWindow& window,
	const sf::Vector2i& startPos,
	const sf::Vector2i& currentPos,
	const CanvasView& view,
	int brushSize) const {
	// ブラシツールはリアルタイム描画なのでプレビューは不要
}

/**
 * ブラシツール：カーソル描画
 * 白色の円形カーソル
 */
void BrushTool::drawCursor(sf::RenderWindow& window,
	const sf::Vector2i& mousePos,
	const CanvasView& view,
	int brushSize,
	int tileSize) const {
	drawBasicCursor(window, mousePos, view, brushSize, tileSize,
		sf::Color(255, 255, 255, 100));
}

// ===== EraserTool 実装 =====

/**
 * 消しゴムツール：描画開始
 * 開始位置のタイルを即座に消去
 */
void EraserTool::onDrawStart(const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// patternIndex = -1 で消去を表す
	applyBrushAt(startPos, brushSize, canvas, view, -1);
}

/**
 * 消しゴムツール：描画継続
 * 前回位置から現在位置まで線に沿って消去
 */
void EraserTool::onDrawContinue(const sf::Vector2i& currentPos,
	const sf::Vector2i& lastPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 前回位置から現在位置まで線に沿って消去
	applyBrushAlongLine(lastPos, currentPos, brushSize, canvas, view, -1);
}

/**
 * 消しゴムツール：描画終了
 * 連続描画ツールなので特別な処理は不要
 */
void EraserTool::onDrawEnd(const sf::Vector2i& endPos,
	const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 消しゴムツールは連続描画なので特別な終了処理は不要
}

/**
 * 消しゴムツール：プレビュー描画
 * 消しゴムは連続描画なのでプレビューは表示しない
 */
void EraserTool::drawPreview(sf::RenderWindow& window,
	const sf::Vector2i& startPos,
	const sf::Vector2i& currentPos,
	const CanvasView& view,
	int brushSize) const {
	// 消しゴムツールはリアルタイム描画なのでプレビューは不要
}

/**
 * 消しゴムツール：カーソル描画
 * 赤色の円形カーソル
 */
void EraserTool::drawCursor(sf::RenderWindow& window,
	const sf::Vector2i& mousePos,
	const CanvasView& view,
	int brushSize,
	int tileSize) const {
	drawBasicCursor(window, mousePos, view, brushSize, tileSize,
		sf::Color(255, 0, 0, 100));
}

// ===== LineTool 実装 =====

/**
 * 直線ツール：描画開始
 * 開始位置を記録するのみ（実際の描画は終了時）
 */
void LineTool::onDrawStart(const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 直線ツールは開始時には何も描画しない
	// プレビュー表示のみで実際の描画は終了時に行う
}

/**
 * 直線ツール：描画継続
 * 連続描画ではないので何もしない
 */
void LineTool::onDrawContinue(const sf::Vector2i& currentPos,
	const sf::Vector2i& lastPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 直線ツールは連続描画をサポートしないので何もしない
}

/**
 * 直線ツール：描画終了
 * 開始位置から終了位置まで直線を描画
 */
void LineTool::onDrawEnd(const sf::Vector2i& endPos,
	const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 開始位置から終了位置まで直線を描画
	applyBrushAlongLine(startPos, endPos, brushSize, canvas, view, patternIndex);
}

/**
 * 直線ツール：プレビュー描画
 * 開始位置から現在位置まで直線のプレビューを表示
 */
void LineTool::drawPreview(sf::RenderWindow& window,
	const sf::Vector2i& startPos,
	const sf::Vector2i& currentPos,
	const CanvasView& view,
	int brushSize) const {

	// プレビュー用の線を描画
	sf::Vertex line[] = {
		sf::Vertex(static_cast<sf::Vector2f>(startPos), sf::Color(255, 255, 255, 150)),
		sf::Vertex(static_cast<sf::Vector2f>(currentPos), sf::Color(255, 255, 255, 150))
	};

	window.draw(line, 2, sf::Lines);

	// 開始点と終了点にマーカーを描画
	float zoom = view.getZoom();
	float markerRadius = 4.0f + zoom;

	// 開始点マーカー
	sf::CircleShape startMarker(markerRadius);
	startMarker.setOrigin(markerRadius, markerRadius);
	startMarker.setPosition(static_cast<sf::Vector2f>(startPos));
	startMarker.setFillColor(sf::Color(0, 255, 0, 150));  // 緑色
	startMarker.setOutlineThickness(1.0f);
	startMarker.setOutlineColor(sf::Color::White);
	window.draw(startMarker);

	// 終了点マーカー
	sf::CircleShape endMarker(markerRadius);
	endMarker.setOrigin(markerRadius, markerRadius);
	endMarker.setPosition(static_cast<sf::Vector2f>(currentPos));
	endMarker.setFillColor(sf::Color(255, 255, 0, 150));  // 黄色
	endMarker.setOutlineThickness(1.0f);
	endMarker.setOutlineColor(sf::Color::White);
	window.draw(endMarker);
}

/**
 * 直線ツール：カーソル描画
 * 十字型のカーソル
 */
void LineTool::drawCursor(sf::RenderWindow& window,
	const sf::Vector2i& mousePos,
	const CanvasView& view,
	int brushSize,
	int tileSize) const {

	float zoom = view.getZoom();
	float crossSize = 10.0f + zoom * 2.0f;
	float lineThickness = 2.0f;

	// 縦線
	sf::RectangleShape vLine(sf::Vector2f(lineThickness, crossSize * 2));
	vLine.setOrigin(lineThickness / 2, crossSize);
	vLine.setPosition(static_cast<sf::Vector2f>(mousePos));
	vLine.setFillColor(sf::Color(255, 255, 255, 200));
	window.draw(vLine);

	// 横線
	sf::RectangleShape hLine(sf::Vector2f(crossSize * 2, lineThickness));
	hLine.setOrigin(crossSize, lineThickness / 2);
	hLine.setPosition(static_cast<sf::Vector2f>(mousePos));
	hLine.setFillColor(sf::Color(255, 255, 255, 200));
	window.draw(hLine);

	// 中心点
	sf::CircleShape centerDot(2.0f);
	centerDot.setOrigin(2.0f, 2.0f);
	centerDot.setPosition(static_cast<sf::Vector2f>(mousePos));
	centerDot.setFillColor(sf::Color::White);
	window.draw(centerDot);
}

// ===== CircleTool 実装 =====

/**
 * 円ツール：描画開始
 * 中心位置を記録するのみ（実際の描画は終了時）
 */
void CircleTool::onDrawStart(const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 円ツールは開始時には何も描画しない
	// プレビュー表示のみで実際の描画は終了時に行う
}

/**
 * 円ツール：描画継続
 * 連続描画ではないので何もしない
 */
void CircleTool::onDrawContinue(const sf::Vector2i& currentPos,
	const sf::Vector2i& lastPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 円ツールは連続描画をサポートしないので何もしない
}

/**
 * 円ツール：描画終了
 * 中心位置から現在位置までの距離を半径として円を描画
 */
void CircleTool::onDrawEnd(const sf::Vector2i& endPos,
	const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {

	// 中心から現在位置までの距離を半径として計算
	float radius = std::sqrt(static_cast<float>(
		(endPos.x - startPos.x) * (endPos.x - startPos.x) +
		(endPos.y - startPos.y) * (endPos.y - startPos.y)
		));

	// 半径が小さすぎる場合は点を描画
	if (radius < 3.0f) {
		applyBrushAt(startPos, brushSize, canvas, view, patternIndex);
	}
	else {
		// 円を描画
		drawCircle(startPos, radius, brushSize, canvas, view, patternIndex);
	}
}

/**
 * 円ツール：プレビュー描画
 * 中心位置から現在位置までの距離を半径として円のプレビューを表示
 */
void CircleTool::drawPreview(sf::RenderWindow& window,
	const sf::Vector2i& startPos,
	const sf::Vector2i& currentPos,
	const CanvasView& view,
	int brushSize) const {

	// 中心から現在位置までの距離を半径として計算
	float radius = std::sqrt(static_cast<float>(
		(currentPos.x - startPos.x) * (currentPos.x - startPos.x) +
		(currentPos.y - startPos.y) * (currentPos.y - startPos.y)
		));

	// プレビュー用の円を描画
	sf::CircleShape previewCircle(radius);
	previewCircle.setOrigin(radius, radius);
	previewCircle.setPosition(static_cast<sf::Vector2f>(startPos));
	previewCircle.setFillColor(sf::Color::Transparent);
	previewCircle.setOutlineThickness(2.0f);
	previewCircle.setOutlineColor(sf::Color(255, 255, 255, 150));
	window.draw(previewCircle);

	// 中心点マーカー
	float markerRadius = 4.0f + view.getZoom();
	sf::CircleShape centerMarker(markerRadius);
	centerMarker.setOrigin(markerRadius, markerRadius);
	centerMarker.setPosition(static_cast<sf::Vector2f>(startPos));
	centerMarker.setFillColor(sf::Color(0, 255, 0, 150));  // 緑色（中心）
	centerMarker.setOutlineThickness(1.0f);
	centerMarker.setOutlineColor(sf::Color::White);
	window.draw(centerMarker);

	// 半径端点マーカー
	sf::CircleShape radiusMarker(markerRadius);
	radiusMarker.setOrigin(markerRadius, markerRadius);
	radiusMarker.setPosition(static_cast<sf::Vector2f>(currentPos));
	radiusMarker.setFillColor(sf::Color(255, 255, 0, 150));  // 黄色（半径端点）
	radiusMarker.setOutlineThickness(1.0f);
	radiusMarker.setOutlineColor(sf::Color::White);
	window.draw(radiusMarker);

	// 半径線
	sf::Vertex radiusLine[] = {
		sf::Vertex(static_cast<sf::Vector2f>(startPos), sf::Color(255, 255, 255, 100)),
		sf::Vertex(static_cast<sf::Vector2f>(currentPos), sf::Color(255, 255, 255, 100))
	};
	window.draw(radiusLine, 2, sf::Lines);

	// 半径値の表示
	std::string radiusText = "R: " + std::to_string(static_cast<int>(radius));
	sf::Text radiusLabel;
	// フォントは main.cpp から渡す必要があるため、ここでは簡略化
	// 実際の実装では sf::Font を引数で受け取るか、静的フォントを使用
}

/**
 * 円ツール：カーソル描画
 * 十字型のカーソル（直線ツールと同じ）
 */
void CircleTool::drawCursor(sf::RenderWindow& window,
	const sf::Vector2i& mousePos,
	const CanvasView& view,
	int brushSize,
	int tileSize) const {

	float zoom = view.getZoom();
	float crossSize = 10.0f + zoom * 2.0f;
	float lineThickness = 2.0f;

	// 縦線
	sf::RectangleShape vLine(sf::Vector2f(lineThickness, crossSize * 2));
	vLine.setOrigin(lineThickness / 2, crossSize);
	vLine.setPosition(static_cast<sf::Vector2f>(mousePos));
	vLine.setFillColor(sf::Color(255, 255, 255, 200));
	window.draw(vLine);

	// 横線
	sf::RectangleShape hLine(sf::Vector2f(crossSize * 2, lineThickness));
	hLine.setOrigin(crossSize, lineThickness / 2);
	hLine.setPosition(static_cast<sf::Vector2f>(mousePos));
	hLine.setFillColor(sf::Color(255, 255, 255, 200));
	window.draw(hLine);

	// 中心点
	sf::CircleShape centerDot(2.0f);
	centerDot.setOrigin(2.0f, 2.0f);
	centerDot.setPosition(static_cast<sf::Vector2f>(mousePos));
	centerDot.setFillColor(sf::Color::White);
	window.draw(centerDot);

	// 小さな円（円ツールであることを示す）
	float indicatorRadius = crossSize * 0.7f;
	sf::CircleShape indicator(indicatorRadius);
	indicator.setOrigin(indicatorRadius, indicatorRadius);
	indicator.setPosition(static_cast<sf::Vector2f>(mousePos));
	indicator.setFillColor(sf::Color::Transparent);
	indicator.setOutlineThickness(1.0f);
	indicator.setOutlineColor(sf::Color(255, 255, 255, 100));
	window.draw(indicator);
}

/**
 * 円周上の点を計算してブラシを適用
 * ブレゼンハムの円描画アルゴリズムを使用
 */
void CircleTool::drawCircle(const sf::Vector2i& centerPos,
	float radius,
	int brushSize,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex) const {

	int r = static_cast<int>(radius);
	int x = 0;
	int y = r;
	int d = 3 - 2 * r;

	// 8方向対称性を利用した円描画
	auto plotCirclePoints = [&](int x, int y) {
		// 8つの対称点にブラシを適用
		std::vector<sf::Vector2i> points = {
			sf::Vector2i(centerPos.x + x, centerPos.y + y),
			sf::Vector2i(centerPos.x - x, centerPos.y + y),
			sf::Vector2i(centerPos.x + x, centerPos.y - y),
			sf::Vector2i(centerPos.x - x, centerPos.y - y),
			sf::Vector2i(centerPos.x + y, centerPos.y + x),
			sf::Vector2i(centerPos.x - y, centerPos.y + x),
			sf::Vector2i(centerPos.x + y, centerPos.y - x),
			sf::Vector2i(centerPos.x - y, centerPos.y - x)
		};

		for (const auto& point : points) {
			if (canvas.containsInView(view, point)) {
				applyBrushAt(point, brushSize, canvas, view, patternIndex);
			}
		}
		};

	// 初期点を描画
	plotCirclePoints(x, y);

	// ブレゼンハムアルゴリズム
	while (y >= x) {
		x++;

		if (d > 0) {
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else {
			d = d + 4 * x + 6;
		}

		plotCirclePoints(x, y);
	}
}

// ===== EllipseTool 実装 =====

/**
 * 楕円ツール：描画開始
 * 開始位置を記録するのみ（実際の描画は終了時）
 */
void EllipseTool::onDrawStart(const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 楕円ツールは開始時には何も描画しない
	// プレビュー表示のみで実際の描画は終了時に行う
}

/**
 * 楕円ツール：描画継続
 * 連続描画ではないので何もしない
 */
void EllipseTool::onDrawContinue(const sf::Vector2i& currentPos,
	const sf::Vector2i& lastPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {
	// 楕円ツールは連続描画をサポートしないので何もしない
}

/**
 * 楕円ツール：描画終了
 * 開始位置と終了位置を対角線とする矩形に内接する楕円を描画
 */
void EllipseTool::onDrawEnd(const sf::Vector2i& endPos,
	const sf::Vector2i& startPos,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex,
	int brushSize) {

	// 矩形が小さすぎる場合は点を描画
	int width = std::abs(endPos.x - startPos.x);
	int height = std::abs(endPos.y - startPos.y);

	if (width < 3 && height < 3) {
		applyBrushAt(startPos, brushSize, canvas, view, patternIndex);
	}
	else {
		// 左上角と右下角を正規化
		sf::Vector2i topLeft(
			std::min(startPos.x, endPos.x),
			std::min(startPos.y, endPos.y)
		);
		sf::Vector2i bottomRight(
			std::max(startPos.x, endPos.x),
			std::max(startPos.y, endPos.y)
		);

		// 楕円を描画
		drawEllipse(topLeft, bottomRight, brushSize, canvas, view, patternIndex);
	}
}

/**
 * 楕円ツール：プレビュー描画
 * 開始位置と現在位置を対角線とする矩形に内接する楕円のプレビューを表示
 */
void EllipseTool::drawPreview(sf::RenderWindow& window,
	const sf::Vector2i& startPos,
	const sf::Vector2i& currentPos,
	const CanvasView& view,
	int brushSize) const {

	// 左上角と右下角を正規化
	sf::Vector2i topLeft(
		std::min(startPos.x, currentPos.x),
		std::min(startPos.y, currentPos.y)
	);
	sf::Vector2i bottomRight(
		std::max(startPos.x, currentPos.x),
		std::max(startPos.y, currentPos.y)
	);

	// 楕円パラメータを計算
	EllipseParams params = calculateEllipseParams(topLeft, bottomRight);

	// プレビュー用の楕円を描画（SFMLの楕円は円を変形させて作成）
	if (params.radiusX > 0 && params.radiusY > 0) {
		// より大きい半径を基準にして円を作成
		float maxRadius = std::max(params.radiusX, params.radiusY);
		sf::CircleShape previewEllipse(maxRadius);
		previewEllipse.setOrigin(maxRadius, maxRadius);
		previewEllipse.setPosition(params.centerX, params.centerY);

		// 楕円にするためにスケールを調整
		float scaleX = params.radiusX / maxRadius;
		float scaleY = params.radiusY / maxRadius;
		previewEllipse.setScale(scaleX, scaleY);

		previewEllipse.setFillColor(sf::Color::Transparent);
		previewEllipse.setOutlineThickness(2.0f / std::min(scaleX, scaleY)); // スケールに応じて調整
		previewEllipse.setOutlineColor(sf::Color(255, 255, 255, 150));
		window.draw(previewEllipse);
	}

	// 境界矩形を描画
	sf::RectangleShape boundingRect(sf::Vector2f(
		bottomRight.x - topLeft.x,
		bottomRight.y - topLeft.y
	));
	boundingRect.setPosition(static_cast<sf::Vector2f>(topLeft));
	boundingRect.setFillColor(sf::Color::Transparent);
	boundingRect.setOutlineThickness(1.0f);
	boundingRect.setOutlineColor(sf::Color(255, 255, 255, 80));
	window.draw(boundingRect);

	// 開始点マーカー
	float markerRadius = 4.0f + view.getZoom();
	sf::CircleShape startMarker(markerRadius);
	startMarker.setOrigin(markerRadius, markerRadius);
	startMarker.setPosition(static_cast<sf::Vector2f>(startPos));
	startMarker.setFillColor(sf::Color(0, 255, 0, 150));  // 緑色（開始点）
	startMarker.setOutlineThickness(1.0f);
	startMarker.setOutlineColor(sf::Color::White);
	window.draw(startMarker);

	// 終了点マーカー
	sf::CircleShape endMarker(markerRadius);
	endMarker.setOrigin(markerRadius, markerRadius);
	endMarker.setPosition(static_cast<sf::Vector2f>(currentPos));
	endMarker.setFillColor(sf::Color(255, 255, 0, 150));  // 黄色（終了点）
	endMarker.setOutlineThickness(1.0f);
	endMarker.setOutlineColor(sf::Color::White);
	window.draw(endMarker);

	// 対角線
	sf::Vertex diagonalLine[] = {
		sf::Vertex(static_cast<sf::Vector2f>(startPos), sf::Color(255, 255, 255, 100)),
		sf::Vertex(static_cast<sf::Vector2f>(currentPos), sf::Color(255, 255, 255, 100))
	};
	window.draw(diagonalLine, 2, sf::Lines);

	// サイズ情報表示
	int width = bottomRight.x - topLeft.x;
	int height = bottomRight.y - topLeft.y;
	std::string sizeText = std::to_string(width) + " x " + std::to_string(height);
	// フォント表示は簡略化（実際の実装では sf::Text を使用）
}

/**
 * 楕円ツール：カーソル描画
 * 十字型のカーソル＋楕円インジケーター
 */
void EllipseTool::drawCursor(sf::RenderWindow& window,
	const sf::Vector2i& mousePos,
	const CanvasView& view,
	int brushSize,
	int tileSize) const {

	float zoom = view.getZoom();
	float crossSize = 10.0f + zoom * 2.0f;
	float lineThickness = 2.0f;

	// 十字カーソル
	sf::RectangleShape vLine(sf::Vector2f(lineThickness, crossSize * 2));
	vLine.setOrigin(lineThickness / 2, crossSize);
	vLine.setPosition(static_cast<sf::Vector2f>(mousePos));
	vLine.setFillColor(sf::Color(255, 255, 255, 200));
	window.draw(vLine);

	sf::RectangleShape hLine(sf::Vector2f(crossSize * 2, lineThickness));
	hLine.setOrigin(crossSize, lineThickness / 2);
	hLine.setPosition(static_cast<sf::Vector2f>(mousePos));
	hLine.setFillColor(sf::Color(255, 255, 255, 200));
	window.draw(hLine);

	// 中心点
	sf::CircleShape centerDot(2.0f);
	centerDot.setOrigin(2.0f, 2.0f);
	centerDot.setPosition(static_cast<sf::Vector2f>(mousePos));
	centerDot.setFillColor(sf::Color::White);
	window.draw(centerDot);

	// 楕円インジケーター（楕円ツールであることを示す）
	float indicatorRadiusX = crossSize * 0.8f;
	float indicatorRadiusY = crossSize * 0.5f;
	sf::CircleShape indicator(indicatorRadiusX);
	indicator.setOrigin(indicatorRadiusX, indicatorRadiusX);
	indicator.setPosition(static_cast<sf::Vector2f>(mousePos));
	indicator.setScale(1.0f, indicatorRadiusY / indicatorRadiusX);
	indicator.setFillColor(sf::Color::Transparent);
	indicator.setOutlineThickness(1.0f);
	indicator.setOutlineColor(sf::Color(255, 255, 255, 100));
	window.draw(indicator);
}

/**
 * 楕円パラメータの計算
 */
EllipseTool::EllipseParams EllipseTool::calculateEllipseParams(const sf::Vector2i& topLeft,
	const sf::Vector2i& bottomRight) const {
	EllipseParams params;
	params.centerX = (topLeft.x + bottomRight.x) / 2.0f;
	params.centerY = (topLeft.y + bottomRight.y) / 2.0f;
	params.radiusX = std::abs(bottomRight.x - topLeft.x) / 2.0f;
	params.radiusY = std::abs(bottomRight.y - topLeft.y) / 2.0f;
	return params;
}

/**
 * 楕円の境界線を描画
 * 中点楕円アルゴリズムを使用
 */
void EllipseTool::drawEllipse(const sf::Vector2i& topLeft,
	const sf::Vector2i& bottomRight,
	int brushSize,
	Canvas& canvas,
	const CanvasView& view,
	int patternIndex) const {

	EllipseParams params = calculateEllipseParams(topLeft, bottomRight);

	int centerX = static_cast<int>(params.centerX);
	int centerY = static_cast<int>(params.centerY);
	int radiusX = static_cast<int>(params.radiusX);
	int radiusY = static_cast<int>(params.radiusY);

	// 楕円描画用の点群を生成
	std::set<std::pair<int, int>> ellipsePoints;

	// 中点楕円アルゴリズム
	int x = 0;
	int y = radiusY;

	// 楕円の初期決定パラメータ
	int radiusX2 = radiusX * radiusX;
	int radiusY2 = radiusY * radiusY;
	int twoRadiusX2 = 2 * radiusX2;
	int twoRadiusY2 = 2 * radiusY2;

	// 領域1の処理
	int p1 = radiusY2 - (radiusX2 * radiusY) + (0.25f * radiusX2);
	int dx = twoRadiusY2 * x;
	int dy = twoRadiusX2 * y;

	// 4つの対称点を追加する関数
	auto addSymmetricPoints = [&](int x, int y) {
		ellipsePoints.insert({ centerX + x, centerY + y });
		ellipsePoints.insert({ centerX - x, centerY + y });
		ellipsePoints.insert({ centerX + x, centerY - y });
		ellipsePoints.insert({ centerX - x, centerY - y });
		};

	// 領域1：dx < dy まで
	while (dx < dy) {
		addSymmetricPoints(x, y);

		if (p1 < 0) {
			x++;
			dx = twoRadiusY2 * x;
			p1 = p1 + dx + radiusY2;
		}
		else {
			x++;
			y--;
			dx = twoRadiusY2 * x;
			dy = twoRadiusX2 * y;
			p1 = p1 + dx - dy + radiusY2;
		}
	}

	// 領域2の初期パラメータ
	int p2 = radiusY2 * (x + 0.5f) * (x + 0.5f) + radiusX2 * (y - 1) * (y - 1) - radiusX2 * radiusY2;

	// 領域2：y >= 0 まで
	while (y >= 0) {
		addSymmetricPoints(x, y);

		if (p2 > 0) {
			y--;
			dy = twoRadiusX2 * y;
			p2 = p2 - dy + radiusX2;
		}
		else {
			y--;
			x++;
			dx = twoRadiusY2 * x;
			dy = twoRadiusX2 * y;
			p2 = p2 + dx - dy + radiusX2;
		}
	}

	// 生成された点にブラシを適用
	for (const auto& point : ellipsePoints) {
		sf::Vector2i screenPos(point.first, point.second);
		if (canvas.containsInView(view, screenPos)) {
			applyBrushAt(screenPos, brushSize, canvas, view, patternIndex);
		}
	}
}


// ===== ToolManager 実装 =====

/**
 * ツールマネージャーのコンストラクタ
 * デフォルトでブラシツールを選択
 */
ToolManager::ToolManager() {
	setTool(ToolType::BRUSH);
}

/**
 * ツールを切り替え
 * 指定されたツールタイプに応じて適切なツールインスタンスを生成
 */
void ToolManager::setTool(ToolType toolType) {
	currentToolType = toolType;

	switch (toolType) {
	case ToolType::BRUSH:
		currentTool = std::make_unique<BrushTool>();
		break;
	case ToolType::ERASER:
		currentTool = std::make_unique<EraserTool>();
		break;
	case ToolType::LINE:
		currentTool = std::make_unique<LineTool>();
		break;
	case ToolType::CIRCLE:  // 新規追加
		currentTool = std::make_unique<CircleTool>();
		break;
	case ToolType::ELLIPSE:  // CIRCLEから変更
		currentTool = std::make_unique<EllipseTool>();
		break;
	case ToolType::LARGE_TILE:  // 新規追加
		currentTool = std::make_unique<LargeTileTool>();
		break;
	}

}

/**
 * 現在のツールタイプを取得
 */
ToolManager::ToolType ToolManager::getCurrentToolType() const {
	return currentToolType;
}

/**
 * 現在のツール名を取得
 */
std::string ToolManager::getCurrentToolName() const {
	if (currentTool) {
		return currentTool->getToolName();
	}
	return "Unknown";
}