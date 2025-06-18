#pragma once
//===== DrawingTools.hpp =====
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <set>

// �O���錾
class Canvas;
class CanvasView;

/**
 * �`��c�[���̊��N���X
 * ���ׂĂ̕`��c�[���i�u���V�A�����S���A�������j�̋��ʃC���^�[�t�F�[�X
 */
class DrawingTool {
public:
	virtual ~DrawingTool() = default;

	/**
	 * �`��J�n���̏���
	 * @param startPos �J�n�ʒu�i�X�N���[�����W�j
	 * @param canvas �`��ΏۃL�����o�X
	 * @param view CanvasView�C���X�^���X
	 * @param patternIndex �I�����ꂽ�p�^�[���C���f�b�N�X
	 * @param brushSize �u���V�T�C�Y
	 */
	virtual void onDrawStart(const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) = 0;

	/**
	 * �`�撆�̏����i�}�E�X�h���b�O���j
	 * @param currentPos ���݈ʒu�i�X�N���[�����W�j
	 * @param lastPos �O��ʒu�i�X�N���[�����W�j
	 * @param canvas �`��ΏۃL�����o�X
	 * @param view CanvasView�C���X�^���X
	 * @param patternIndex �I�����ꂽ�p�^�[���C���f�b�N�X
	 * @param brushSize �u���V�T�C�Y
	 */
	virtual void onDrawContinue(const sf::Vector2i& currentPos,
		const sf::Vector2i& lastPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) = 0;

	/**
	 * �`��I�����̏���
	 * @param endPos �I���ʒu�i�X�N���[�����W�j
	 * @param startPos �J�n�ʒu�i�X�N���[�����W�j
	 * @param canvas �`��ΏۃL�����o�X
	 * @param view CanvasView�C���X�^���X
	 * @param patternIndex �I�����ꂽ�p�^�[���C���f�b�N�X
	 * @param brushSize �u���V�T�C�Y
	 */
	virtual void onDrawEnd(const sf::Vector2i& endPos,
		const sf::Vector2i& startPos,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex,
		int brushSize) = 0;

	/**
	 * �v���r���[�`��i���ۂɂ͕`�悹���A��ʏ�Ƀv���r���[�\���j
	 * @param window �`��E�B���h�E
	 * @param startPos �J�n�ʒu�i�X�N���[�����W�j
	 * @param currentPos ���݈ʒu�i�X�N���[�����W�j
	 * @param view CanvasView�C���X�^���X
	 * @param brushSize �u���V�T�C�Y
	 */
	virtual void drawPreview(sf::RenderWindow& window,
		const sf::Vector2i& startPos,
		const sf::Vector2i& currentPos,
		const CanvasView& view,
		int brushSize) const = 0;

	/**
	 * �J�[�\���`��
	 * @param window �`��E�B���h�E
	 * @param mousePos �}�E�X�ʒu�i�X�N���[�����W�j
	 * @param view CanvasView�C���X�^���X
	 * @param brushSize �u���V�T�C�Y
	 * @param tileSize �^�C���T�C�Y
	 */
	virtual void drawCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize) const = 0;

	/**
	 * �c�[�������擾
	 */
	virtual std::string getToolName() const = 0;

	/**
	 * ���̃c�[�����A���`����T�|�[�g���邩
	 * �i�u���V�Ftrue�A�����Ffalse�j
	 */
	virtual bool supportsContinuousDrawing() const = 0;

protected:
	/**
	 * �u���V���w��ʒu�ɓK�p����w���p�[�֐�
	 * @param position �ʒu�i�X�N���[�����W�j
	 * @param brushSize �u���V�T�C�Y
	 * @param canvas �`��ΏۃL�����o�X
	 * @param view CanvasView�C���X�^���X
	 * @param patternIndex �p�^�[���C���f�b�N�X�i-1�ŏ����j
	 */
	void applyBrushAt(const sf::Vector2i& position,
		int brushSize,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex) const;

	/**
	 * ���ɉ����ău���V��K�p����w���p�[�֐�
	 * @param startPos �J�n�ʒu�i�X�N���[�����W�j
	 * @param endPos �I���ʒu�i�X�N���[�����W�j
	 * @param brushSize �u���V�T�C�Y
	 * @param canvas �`��ΏۃL�����o�X
	 * @param view CanvasView�C���X�^���X
	 * @param patternIndex �p�^�[���C���f�b�N�X�i-1�ŏ����j
	 */
	void applyBrushAlongLine(const sf::Vector2i& startPos,
		const sf::Vector2i& endPos,
		int brushSize,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex) const;

	/**
	 * ��{�I�ȃu���V�J�[�\����`�悷��w���p�[�֐�
	 * @param window �`��E�B���h�E
	 * @param mousePos �}�E�X�ʒu
	 * @param view CanvasView�C���X�^���X
	 * @param brushSize �u���V�T�C�Y
	 * @param tileSize �^�C���T�C�Y
	 * @param color �J�[�\���F
	 */
	void drawBasicCursor(sf::RenderWindow& window,
		const sf::Vector2i& mousePos,
		const CanvasView& view,
		int brushSize,
		int tileSize,
		const sf::Color& color) const;
};

// ===== ��ۃc�[���N���X =====

/**
 * �u���V�c�[��
 * �]���̃u���V�@�\������
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
 * �����S���c�[��
 * �]���̏����S���@�\������
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
 * �����c�[��
 * �N���b�N���h���b�O�Œ�����`��
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
 * �~�c�[��
 * �N���b�N���h���b�O�ŉ~��`��i���S���甼�a���w��j
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
	 * �~����̓_���v�Z���ău���V��K�p
	 * @param centerPos ���S�ʒu�i�X�N���[�����W�j
	 * @param radius ���a�i�X�N���[�����W�j
	 * @param brushSize �u���V�T�C�Y
	 * @param canvas �`��ΏۃL�����o�X
	 * @param view CanvasView�C���X�^���X
	 * @param patternIndex �p�^�[���C���f�b�N�X
	 */
	void drawCircle(const sf::Vector2i& centerPos,
		float radius,
		int brushSize,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex) const;
};


/**
 * �ȉ~�c�[���i�~�c�[����u�������j
 * �N���b�N���h���b�O�őȉ~��`��i�Ίp���őȉ~���`�j
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
	 * �ȉ~�̋��E����`��
	 * @param topLeft �ȉ~���͂ދ�`�̍���p�i�X�N���[�����W�j
	 * @param bottomRight �ȉ~���͂ދ�`�̉E���p�i�X�N���[�����W�j
	 * @param brushSize �u���V�T�C�Y
	 * @param canvas �`��ΏۃL�����o�X
	 * @param view CanvasView�C���X�^���X
	 * @param patternIndex �p�^�[���C���f�b�N�X
	 */
	void drawEllipse(const sf::Vector2i& topLeft,
		const sf::Vector2i& bottomRight,
		int brushSize,
		Canvas& canvas,
		const CanvasView& view,
		int patternIndex) const;

	/**
	 * �ȉ~�p�����[�^�̌v�Z
	 * @param topLeft ����p
	 * @param bottomRight �E���p
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
 * �c�[���Ǘ��N���X
 * �e�c�[���̐����E�؂�ւ����Ǘ�
 */
class ToolManager {
private:
	std::unique_ptr<DrawingTool> currentTool;

public:
	enum class ToolType {
		BRUSH,
		ERASER,
		LINE,
		CIRCLE,  // �V�K�ǉ�
		 ELLIPSE, // �V�K�ǉ�
		LARGE_TILE  // �V�K�ǉ�
	};

	ToolManager();

	/**
	 * �c�[����؂�ւ�
	 * @param toolType �؂�ւ���̃c�[���^�C�v
	 */
	void setTool(ToolType toolType);

	/**
	 * ���݂̃c�[�����擾
	 */
	DrawingTool* getCurrentTool() const { return currentTool.get(); }

	/**
	 * ���݂̃c�[���^�C�v���擾
	 */
	ToolType getCurrentToolType() const;

	/**
	 * �c�[�������擾
	 */
	std::string getCurrentToolName() const;

private:
	ToolType currentToolType = ToolType::BRUSH;
};