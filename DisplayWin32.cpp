#include "DisplayWin32.h"
#include <iostream>
#include <cstdio>

LRESULT CALLBACK DisplayWin32::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
    {
        // Print key codes to the console (matches your standalone example).
        std::cout << "Key: " << static_cast<unsigned int>(wParam) << std::endl;
        if (static_cast<unsigned int>(wParam) == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        return 0;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

DisplayWin32::DisplayWin32()
    : clientWidth(1280),
    clientHeight(720),
    hwnd(nullptr),
    hInstance(GetModuleHandle(nullptr)),
    applicationName(L"My3DApp")
{
#pragma region Window init
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    auto screenWidth = 800;
    auto screenHeight = 800;

    RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

    auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
    auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

    hwnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
        dwStyle,
        posX, posY,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    ShowCursor(true);

    // Create a console so prints to std::cout are visible (useful for GUI subsystem builds).
    // If a parent console exists AttachConsole will succeed; otherwise AllocConsole creates one.
    if (AttachConsole(ATTACH_PARENT_PROCESS) == 0) {
        AllocConsole();
    }
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    if (fp) {
        std::ios::sync_with_stdio(true);
        std::cout.setf(std::ios::unitbuf);
    }
#pragma endregion Window init
}