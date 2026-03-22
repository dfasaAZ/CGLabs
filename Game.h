#pragma once
#include "DisplayWin32.h"
#include <vector>
#include "GameComponent.h"
#include <wrl.h>
#include "TriangleComponent.h"
class Game
{
private:
	void CreateBackBuffer();
	
protected:
	void DestroyResources();
	void Draw();
	void EndFrame();
	void Initialize();
	void PrepareFrame();
	void PrepareResources();
	void Update();
	void UpdateInternal();
	
	public:
		DisplayWin32 Display;
		IDXGISwapChain* swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> Device;
		ID3D11DeviceContext* context;
		std::vector<GameComponent*> Components;
		TriangleComponent* triangleComponent;
		ID3D11RenderTargetView* rtv;

	Game();
	~Game();
	void Exit();
	void MessageHandler();
	void RestoreTargets();
	void Run();


};

