#include "DisplayWin32.h"
#include "Game.h" 
#include <iostream>
#include <cstdio>
#include <vector>

Game* DisplayWin32::gameInstance = nullptr;
// Ловят нажатия и другие события
LRESULT CALLBACK DisplayWin32::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
    {
        // Каким-то образом из lParam извлекают данные
        gameInstance->inputDevice.OnKeyDown({
            static_cast<unsigned short>((lParam >> 16) & 0xFF), // MakeCode
            static_cast<unsigned short>((lParam >> 24) & 0xFF), // Flags
            static_cast<unsigned short>(wParam), // VKey
            msg
            });
        if (static_cast<unsigned int>(wParam) == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        return 0;
    }
    case WM_KEYUP:
    {
        gameInstance->inputDevice.OnKeyUp({
            static_cast<unsigned short>((lParam >> 16) & 0xFF),
            static_cast<unsigned short>((lParam >> 24) & 0xFF),
            static_cast<unsigned short>(wParam),
            msg
            });
        return 0;
    }
    case WM_INPUT:
    {
        // Get size of raw input
        UINT dwSize = 0;
        if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER)) == (UINT)-1) {
            break;
        }

        std::vector<BYTE> lpb(dwSize);
        if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
            break;
        }

        RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.data());

        if (raw->header.dwType == RIM_TYPEKEYBOARD)
        {
            // Forward raw keyboard events to InputDevice (make sure InputDevice exists)
            if (raw->data.keyboard.Message == WM_KEYDOWN || raw->data.keyboard.Message == WM_SYSKEYDOWN) {
                gameInstance->inputDevice.OnKeyDown({
                    raw->data.keyboard.MakeCode,
                    raw->data.keyboard.Flags,
                    static_cast<unsigned short>(raw->data.keyboard.VKey),
                    static_cast<unsigned int>(raw->data.keyboard.Message)
                    });
            }
            else if (raw->data.keyboard.Message == WM_KEYUP || raw->data.keyboard.Message == WM_SYSKEYUP) {
                gameInstance->inputDevice.OnKeyUp({
                    raw->data.keyboard.MakeCode,
                    raw->data.keyboard.Flags,
                    static_cast<unsigned short>(raw->data.keyboard.VKey),
                    static_cast<unsigned int>(raw->data.keyboard.Message)
                    });
            }
        }
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            // Forward raw mouse events to InputDevice
            gameInstance->inputDevice.OnMouseMove({
                static_cast<int>(raw->data.mouse.usFlags),
                static_cast<int>(raw->data.mouse.usButtonFlags),
                static_cast<int>(raw->data.mouse.ulExtraInformation),
                static_cast<int>(raw->data.mouse.ulRawButtons),
                static_cast<int>(static_cast<SHORT>(raw->data.mouse.usButtonData)),
                static_cast<int>(raw->data.mouse.lLastX),
                static_cast<int>(raw->data.mouse.lLastY)
                });
        }

        return 0;
    }
    case WM_SIZE:
        //Пока не работает
        if (wParam != SIZE_MINIMIZED and gameInstance != nullptr) {
            gameInstance->OnResize();
        }
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

DisplayWin32::DisplayWin32()
    : clientWidth(800),
    clientHeight(800),
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

    auto screenWidth = this->clientWidth;
    auto screenHeight = this->clientHeight;

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