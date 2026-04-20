#pragma once
#include "DisplayWin32.h"
#include <vector>
#include "GameComponent.h"
#include <wrl.h>
#include "TriangleComponent.h"
#include "InputDevice.h"
#include "Camera.h"

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
		void ballCheck();
		void spawnBall();

		//Глубина
		void CreateDepthBuffer();
		void BindDepthBuffer();

		//Камера
		Camera* mainCamera;
		bool freeLookActive;
		POINT lastMousePos;
		int currentComponentIndex = 0;

		//Мишура
		DisplayWin32 Display;
		IDXGISwapChain* swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> Device;
		InputDevice inputDevice;
		ID3D11DeviceContext* context;
		ID3D11RenderTargetView* rtv;
		ID3D11Texture2D* depthStencilBuffer = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;

		//Игра
		std::vector<GameComponent*> Components;
		std::vector<GameComponent*> players;
		TriangleComponent* triangleComponent;
		PongBall* ball;
		int player1Score = 0;
		int player2Score = 0;

	Game();
	~Game();
	void Exit();
	void MessageHandler();
	void RestoreTargets();
	void Run();

	void viewportFromDisplay();


};

