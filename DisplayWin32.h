#ifndef DISPLAY_WIN32_H
#define DISPLAY_WIN32_H

#include <windows.h>
class Game;

class DisplayWin32
{
private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static Game* gameInstance;
public:
    int clientWidth;
    int clientHeight;
    HWND hwnd;
    HINSTANCE hInstance;
    WNDCLASSEX wc;
    LPCWSTR applicationName;

    DisplayWin32();
    void setGameInstance(Game* game) {
        gameInstance = game;
	}
};

#endif // DISPLAY_WIN32_H
