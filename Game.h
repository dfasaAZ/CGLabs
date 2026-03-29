#pragma once
#include "DisplayWin32.h"
#include <vector>
#include "GameComponent.h"
#include <wrl.h>
#include "TriangleComponent.h"
#include "InputDevice.h"

class PongBall;

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
		void OnResize();
		void processInput(float deltaTime);
		void spawnBall();
		DisplayWin32 Display;
		IDXGISwapChain* swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> Device;
		InputDevice inputDevice;
		ID3D11DeviceContext* context;
		std::vector<GameComponent*> Components;
		std::vector<GameComponent*> players;
		TriangleComponent* triangleComponent;
		PongBall* ball;
		ID3D11RenderTargetView* rtv;

	Game();
	~Game();
	void Exit();
	void MessageHandler();
	void RestoreTargets();
	void Run();

	void viewportFromDisplay();


};

