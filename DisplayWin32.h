#ifndef DISPLAY_WIN32_H
#define DISPLAY_WIN32_H

#include <windows.h>

class DisplayWin32
{
private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    int clientWidth;
    int clientHeight;
    HWND hwnd;
    HINSTANCE hInstance;
    WNDCLASSEX wc;
    LPCWSTR applicationName;

    DisplayWin32();
};

#endif // DISPLAY_WIN32_H
