#include "Game.h"
#include "DisplayWin32.h"
#include <d3d11.h>
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <chrono>
#include <d3dcompiler.h>
#include <iostream>
#include <cmath>

Game::Game()
{
	//Display = DisplayWin32();
}

Game::~Game()
{
}
void Game::Initialize() {
	
	PrepareResources();
	CreateBackBuffer();

	if (context && rtv) {
		context->OMSetRenderTargets(1, &rtv, nullptr);
	}

	// Создаем треугольники
	DirectX::XMFLOAT4 points[6] = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),

	};
	triangleComponent = new TriangleComponent(this,points);
	Components.push_back(triangleComponent);

	DirectX::XMFLOAT4 points1[6] = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
	};
	triangleComponent = new TriangleComponent(this, points1);
	Components.push_back(triangleComponent);

}
void Game::CreateBackBuffer() {
	ID3D11Texture2D* backTex;
	swapChain->GetBuffer(0, IID_PPV_ARGS(&backTex));
	Device->CreateRenderTargetView(backTex, nullptr, &rtv);
	backTex->Release();
}
void Game::Run() {
	Initialize();
	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;
	MSG msg = {};
	bool isExitRequested = false;

	while (!isExitRequested) {
	
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			isExitRequested = true;
			break;
		}

		auto curTime = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
		PrevTime = curTime;

		totalTime += deltaTime;
		frameCount++;

		//FPS
		if (totalTime > 1.0f) {
			float fps = frameCount / totalTime;
			totalTime -= 1.0f;
			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(Display.hwnd, text);
			frameCount = 0;
		}

		if (context) {
			context->ClearState();
		}

		if (context && rtv) {
			context->OMSetRenderTargets(1, &rtv, nullptr);
		}

		float red = fmodf(totalTime, 1.0f);
		float color[] = { red, 0.1f, 0.1f, 1.0f };
		context->ClearRenderTargetView(rtv, color);

		for (auto component : Components) {
			component->Draw();
		}

		swapChain->Present(1, 0);
	}
	
}
void Game::PrepareResources() {
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = Display.clientWidth;
	swapDesc.BufferDesc.Height = Display.clientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = Display.hwnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&Device,
		nullptr,
		&context);

	if (FAILED(res)) {
		MessageBox(Display.hwnd, L"Failed to create D3D11 device", L"Error", MB_OK);
		return;
	}
}