#pragma once
class AppSettings {
public:
    static int tileSize;
    static int canvasWidth;
    static int canvasHeight;

    // �ݒ���X�V
    static void updateSettings(int newTileSize, int newCanvasWidth, int newCanvasHeight) {
        tileSize = newTileSize;
        canvasWidth = newCanvasWidth;
        canvasHeight = newCanvasHeight;
    }
};
