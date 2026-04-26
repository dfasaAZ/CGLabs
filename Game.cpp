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
#include "RectangleComponent.h"
#include "PongBall.h"
#include "CubeComponent.h"
#include "SphereComponent.h"
#include "Planet.h"

Game::Game() :inputDevice(this)
{
	Display.setGameInstance(this);
	ball = nullptr;
}

Game::~Game()
{
}

void Game::spawnBall()
{
	if (ball != nullptr) {
		delete ball;
	}
	ball = new PongBall(this, DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
	ball->setPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	float randomY = (rand() % 2 == 0 ? 1.0f : -1.0f) * (0.5f + (rand() / (float)RAND_MAX) * 0.5f);
	ball->getPhysics()->setVelocity(DirectX::XMFLOAT3(randomY*-1, randomY, 0.0f)); // Начальная скорость мяча
	Components.push_back(ball);

}


void Game::Initialize() {
	
	PrepareResources();
	CreateBackBuffer();

	if (context && rtv) {
		BindDepthBuffer();
	}
	mainCamera = new Camera(Display.clientWidth / Display.clientHeight);
	//Для нижней стороны
	RectangleComponent* rect = new RectangleComponent(this, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	rect->setScale(DirectX::XMFLOAT3(0.325f, 0.0325f, 1.0f));
	rect->setPosition(DirectX::XMFLOAT3(-0.5f, -1.0f, 0.0f));
	rect->setPositionConstraint(1.0f, 0.0f, 0.0f);
	rect->setRotationConstraint(0.0f, 0.0f, 0.0f);
	rect->getPhysics()->setKinematic(false);
	rect->getPhysics()->setGravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	rect->getPhysics()->setRestitution(1.0f);
	Components.push_back(rect);
	players.push_back(rect);

	rect = new RectangleComponent(this, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	rect->setScale(DirectX::XMFLOAT3(0.325f, 0.0325f, 1.0f));
	rect->setPosition(DirectX::XMFLOAT3(0.5f, -1.0f, 0.0f));
	rect->setPositionConstraint(1.0f, 0.0f, 0.0f);
	rect->setRotationConstraint(0.0f, 0.0f, 0.0f);
	rect->getPhysics()->setKinematic(false);
	rect->getPhysics()->setGravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	rect->getPhysics()->setRestitution(1.0f);
	Components.push_back(rect);
	players.push_back(rect);

	//Для верхней стороны
	rect = new RectangleComponent(this, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	rect->setScale(DirectX::XMFLOAT3(0.325f, 0.0325f, 1.0f));
	rect->setPosition(DirectX::XMFLOAT3(-0.5f, 1.0f, 0.0f));
	rect->setPositionConstraint(1.0f, 0.0f, 0.0f);
	rect->setRotationConstraint(0.0f, 0.0f, 0.0f);
	rect->getPhysics()->setKinematic(false);
	rect->getPhysics()->setGravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	rect->getPhysics()->setRestitution(1.0f);
	Components.push_back(rect);
	players.push_back(rect);

	rect = new RectangleComponent(this, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	rect->setScale(DirectX::XMFLOAT3(0.325f, 0.0325f, 1.0f));
	rect->setPosition(DirectX::XMFLOAT3(0.5f, 1.0f, 0.0f));
	rect->setPositionConstraint(1.0f, 0.0f, 0.0f);
	rect->setRotationConstraint(0.0f, 0.0f, 0.0f);
	rect->getPhysics()->setKinematic(false);
	rect->getPhysics()->setGravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	rect->getPhysics()->setRestitution(1.0f);
	Components.push_back(rect);
	players.push_back(rect);

	

	// Стены
	// Top wall
	rect = new RectangleComponent(this, DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	rect->setScale(DirectX::XMFLOAT3(2.5f, 0.065f, 1.0f));
	rect->setPosition(DirectX::XMFLOAT3(0.0f, 1.25f, 0.0f));
	rect->setPositionConstraint(0.0f, 0.0f, 0.0f);
	rect->getPhysics()->setKinematic(true);
	rect->getPhysics()->setRestitution(1.0f);
	Components.push_back(rect);

	// Bottom wall
	rect = new RectangleComponent(this, DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	rect->setScale(DirectX::XMFLOAT3(2.5f, 0.065f, 1.0f));
	rect->setPosition(DirectX::XMFLOAT3(0.0f, -1.25f, 0.0f));
	rect->setPositionConstraint(0.0f, 0.0f, 0.0f);
	rect->getPhysics()->setKinematic(true);
	rect->getPhysics()->setRestitution(1.0f);
	Components.push_back(rect);

	// Left wall
	rect = new RectangleComponent(this, DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	rect->setScale(DirectX::XMFLOAT3(0.135f, 2.5f, 1.0f));
	rect->setPosition(DirectX::XMFLOAT3(-1.05f, 0.0f, 0.0f));
	rect->setPositionConstraint(0.0f, 0.0f, 0.0f);
	rect->getPhysics()->setKinematic(true);
	rect->getPhysics()->setRestitution(1.0f);
	Components.push_back(rect);

	// Right wall
	rect = new RectangleComponent(this, DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	rect->setScale(DirectX::XMFLOAT3(0.135f, 2.5f, 1.0f));
	rect->setPosition(DirectX::XMFLOAT3(1.05f, 0.0f, 0.0f));
	rect->setPositionConstraint(0.0f, 0.0f, 0.0f);
	rect->getPhysics()->setKinematic(true);
	rect->getPhysics()->setRestitution(1.0f);
	Components.push_back(rect);

	// Central Star (Sun)
	MeshComponent* sunMesh = new SphereComponent(this, DirectX::XMFLOAT4(1.0f, 0.8f, 0.2f, 1.0f));
	Planet* sun = new Planet(this, sunMesh, 0.0f, -1.0f);
	sun->setPosition(0.0f, 0.0f, 0.0f);
	sun->setScale(1.0f, 1.0f, 1.0f);
	Components.push_back(sun);
	// Меркуриц
	MeshComponent* mercuryMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.6f, 0.4f, 0.3f, 1.0f));
	Planet* mercury = new Planet(this, mercuryMesh, 0.1f, 5.0f);
	mercury->setPosition(5.0f, 0.0f, 0.0f);
	mercury->setScale(0.6f, 0.6f, 0.6f);
	sun->addChild(mercury);
	Components.push_back(mercury);

	// Венера
	MeshComponent* venusMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.9f, 0.7f, 0.4f, 1.0f));
	Planet* venus = new Planet(this, venusMesh, 0.2f, 8.0f);
	venus->setPosition(8.0f, 0.0f, 0.0f);
	venus->setScale(0.9f, 0.9f, 0.9f);
	sun->addChild(venus);
	Components.push_back(venus);

	// Земля
	MeshComponent* earthMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.2f, 0.5f, 0.8f, 1.0f));
	Planet* earth = new Planet(this, earthMesh, 0.5f, 11.0f);
	earth->setPosition(11.0f, 0.0f, 0.0f);
	earth->setScale(1.0f, 1.0f, 1.0f);
	sun->addChild(earth);
	Components.push_back(earth);

	// Луна Земли
	MeshComponent* lunaMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	Planet* moon1 = new Planet(this, lunaMesh, 0.5f, 0.0f);
	moon1->setPosition(2.5f, 0.0f, 0.0f);
	moon1->setScale(0.27f, 0.27f, 0.27f);
	earth->addChild(moon1);
	Components.push_back(moon1);

	MeshComponent* earthMoon2Mesh = new SphereComponent(this, DirectX::XMFLOAT4(0.7f, 0.7f, 0.8f, 1.0f));
	Planet* moon1b = new Planet(this, earthMoon2Mesh, 1.8f, 0.0f);
	moon1b->setPosition(-3.2f, 1.5f, 0.0f);
	moon1b->setScale(0.18f, 0.18f, 0.18f);
	earth->addChild(moon1b);
	Components.push_back(moon1b);

	// Марс
	MeshComponent* marsMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.8f, 0.3f, 0.2f, 1.0f));
	Planet* mars = new Planet(this, marsMesh, 0.8f, 14.0f);
	mars->setPosition(14.0f, 0.0f, 0.0f);
	mars->setScale(0.85f, 0.85f, 0.85f);
	sun->addChild(mars);
	Components.push_back(mars);

	// Луны Марса
	MeshComponent* phobosMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.7f, 0.6f, 0.5f, 1.0f));
	Planet* phobos = new Planet(this, phobosMesh, 0.8f, 0.0f);
	phobos->setPosition(1.8f, 0.5f, 0.0f);
	phobos->setScale(0.12f, 0.12f, 0.12f);
	mars->addChild(phobos);
	Components.push_back(phobos);

	MeshComponent* deimosMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.6f, 0.5f, 0.4f, 1.0f));
	Planet* deimos = new Planet(this, deimosMesh, 0.25f, 0.0f);
	deimos->setPosition(-2.2f, -0.8f, 1.2f);
	deimos->setScale(0.1f, 0.1f, 0.1f);
	mars->addChild(deimos);
	Components.push_back(deimos);

	// Юпитер
	MeshComponent* jupiterMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.7f, 0.6f, 0.5f, 1.0f));
	Planet* jupiter = new Planet(this, jupiterMesh, 0.2f, 18.0f);
	jupiter->setPosition(18.0f, 0.0f, 0.0f);
	jupiter->setScale(2.2f, 2.2f, 2.2f);
	sun->addChild(jupiter);
	Components.push_back(jupiter);

	// Луны Юпитера
	MeshComponent* ioSternMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.9f, 0.7f, 0.3f, 1.0f));
	Planet* io = new Planet(this, ioSternMesh, 1.5f, 0.0f);
	io->setPosition(3.5f, 0.0f, 0.0f);
	io->setScale(0.4f, 0.4f, 0.4f);
	jupiter->addChild(io);
	Components.push_back(io);

	MeshComponent* europaMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.7f, 0.8f, 0.9f, 1.0f));
	Planet* europa = new Planet(this, europaMesh, 3.0f, 0.0f);
	europa->setPosition(-4.2f, 1.2f, 1.5f);
	europa->setScale(0.35f, 0.35f, 0.35f);
	jupiter->addChild(europa);
	Components.push_back(europa);

	MeshComponent* ganymedeMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.6f, 0.7f, 0.6f, 1.0f));
	Planet* ganymede = new Planet(this, ganymedeMesh, 3.8f, 0.0f);
	ganymede->setPosition(5.8f, -1.0f, 2.0f);
	ganymede->setScale(0.45f, 0.45f, 0.45f);
	jupiter->addChild(ganymede);
	Components.push_back(ganymede);

	MeshComponent* callistoMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.5f, 0.5f, 0.4f, 1.0f));
	Planet* callisto = new Planet(this, callistoMesh, 4.2f, 0.0f);
	callisto->setPosition(-7.0f, -1.5f, -1.8f);
	callisto->setScale(0.42f, 0.42f, 0.42f);
	jupiter->addChild(callisto);
	Components.push_back(callisto);

	// Астероиды
	for (int i = 0; i < 30; i++) {
	
		float randomValue = static_cast<float>(rand()) / RAND_MAX;
		float angle = randomValue * 2.0f * 3.14159f;

		float radius = 6.0f + (i * 0.1f);
		MeshComponent* asteroidMesh = new SphereComponent(this, DirectX::XMFLOAT4(0.5f, 0.4f, 0.3f, 1.0f));
		Planet* asteroid = new Planet(this, asteroidMesh, 1.0f, 0.0f);
		asteroid->setPosition(cos(angle) * radius, sin(angle) * radius * 0.2f, sin(angle) * radius * 0.5f);
		asteroid->setScale(0.08f, 0.08f, 0.08f);
		sun->addChild(asteroid);
		Components.push_back(asteroid);

	}
	
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

	// Главный цикл игры
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
		
		processInput(deltaTime);

		

		

		if (context) {
			context->ClearState();
		}

		if (context && rtv) {
			context->OMSetRenderTargets(1, &rtv, depthStencilView);
		}
		//Красная моргалка
		float red = fmodf(totalTime, 1.0f);
		float color[] = { red, 0.1f, 0.1f, 1.0f };
		context->ClearRenderTargetView(rtv, color);

		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		
		viewportFromDisplay();
		mainCamera->Update(deltaTime);
		for (size_t i = 0; i < Components.size(); ++i) {
			for (size_t j = i + 1; j < Components.size(); ++j) {
				if (Components[i]->checkCollision(Components[j])) {
					Components[i]->resolveCollision(Components[j]);
				}
			}
		}
		ballCheck();

		//Костыль для отскоков
		for (auto component : players) {
			component->getPhysics()->setVelocity(DirectX::XMFLOAT3(0,0,0));
		}
		for (auto component : Components) {
			component->update(deltaTime);
			component->updatePhysics(deltaTime);
			component->draw();
		}

		swapChain->Present(1, 0);
	}
	
}
void Game::viewportFromDisplay() {
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(Display.clientWidth);
	viewport.Height = static_cast<float>(Display.clientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);
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
	CreateDepthBuffer();
}
void Game::OnResize() {
	if (context) {
		context->OMSetRenderTargets(0, nullptr, nullptr);
	}
	if (rtv) {
		rtv->Release();
		rtv = nullptr;
	}
	swapChain->ResizeBuffers(0, Display.clientWidth, Display.clientHeight, DXGI_FORMAT_UNKNOWN, 0);
	CreateBackBuffer();
}
void Game::processInput(float deltaTime) {

	float speed = 5.0f;
	
#pragma region CameraControls
	float moveSpeed = speed * deltaTime*0.15f;
	float rotSpeed = 0.6f * deltaTime;
	int objectCount = static_cast<int>(Components.size());
	currentComponentIndex = (currentComponentIndex + objectCount) % objectCount;

	// Camera controls (only in free mode)
	if (mainCamera->GetMode() == CameraMode::Free) {
		if (inputDevice.IsKeyDown(Keys::W)) mainCamera->MoveForward(moveSpeed);
		if (inputDevice.IsKeyDown(Keys::S)) mainCamera->MoveForward(-moveSpeed);
		if (inputDevice.IsKeyDown(Keys::A)) mainCamera->MoveRight(-moveSpeed);
		if (inputDevice.IsKeyDown(Keys::D)) mainCamera->MoveRight(moveSpeed);
		if (inputDevice.IsKeyDown(Keys::Q)) mainCamera->MoveUp(-moveSpeed);
		if (inputDevice.IsKeyDown(Keys::E)) mainCamera->MoveUp(moveSpeed);

		// Mouse look when right button held
		if (inputDevice.IsKeyDown(Keys::RightButton)) {
			DirectX::SimpleMath::Vector2 mouseDelta = inputDevice.MouseOffset;
			mainCamera->Yaw(mouseDelta.x * rotSpeed);
			mainCamera->Pitch(-mouseDelta.y * rotSpeed);
		}
	}

	// Mode switching
	if (inputDevice.IsKeyPressed(Keys::NumPad1)) { mainCamera->SetMode(CameraMode::Free); printf("Set camera mode"); }
	if (inputDevice.IsKeyPressed(Keys::NumPad2) && !Components.empty())
		mainCamera->AttachToFP(Components[++currentComponentIndex % Components.size()]);
	if (inputDevice.IsKeyPressed(Keys::NumPad3) && !Components.empty())
		mainCamera->AttachToTP(Components[++currentComponentIndex % Components.size()]);
#pragma endregion

#pragma region ObjectControls
	if (mainCamera->GetMode() == CameraMode::FirstPerson || mainCamera->GetMode() == CameraMode::ThirdPerson) {
		float moveSpeed = speed * deltaTime * 10.15f;
		Planet* currentPlanet = dynamic_cast<Planet*>(Components[currentComponentIndex % Components.size()]);

		if (currentPlanet) {
			currentPlanet->isControlledByPlayer = true;
			if (inputDevice.IsKeyDown(Keys::W)) {
				currentPlanet->translate(DirectX::XMFLOAT3(0, 0, moveSpeed));
			} // Forward
			if (inputDevice.IsKeyDown(Keys::S))
				currentPlanet->translate(DirectX::XMFLOAT3(0, 0, -moveSpeed)); // Backward
			if (inputDevice.IsKeyDown(Keys::A))
				currentPlanet->translate(DirectX::XMFLOAT3(-moveSpeed, 0, 0)); // Left
			if (inputDevice.IsKeyDown(Keys::D))
				currentPlanet->translate(DirectX::XMFLOAT3(moveSpeed, 0, 0));  // Right
			if (inputDevice.IsKeyDown(Keys::Q))
				currentPlanet->translate(DirectX::XMFLOAT3(0, moveSpeed, 0));  // Up
			if (inputDevice.IsKeyDown(Keys::E))
				currentPlanet->translate(DirectX::XMFLOAT3(0, -moveSpeed, 0)); // Down
		}
		// Mouse look
		DirectX::SimpleMath::Vector2 mouseDelta = inputDevice.MouseOffset;
		mainCamera->Yaw(mouseDelta.x * rotSpeed);
		mainCamera->Pitch(-mouseDelta.y * rotSpeed);

	}
	if (players.size() >= 4) {
		struct ControlScheme {
			int playerIndex;
			Keys up, down, left, right;
		};

		ControlScheme schemes[] = {
			{0, Keys::None, Keys::None, Keys::W, Keys::E},        // Нижняя левая
			{1, Keys::None, Keys::None, Keys::S, Keys::D},        // Нижняя правая
			{2, Keys::None, Keys::None, Keys::I, Keys::O},        // Верхняя левая
			{3, Keys::None, Keys::None, Keys::K, Keys::L}		 // Верхняя правая
		};

		for (const auto& scheme : schemes) {
			GameComponent* comp = dynamic_cast<GameComponent*>(players[scheme.playerIndex]);
			if (!comp) continue;

			// Вертикальное и горизонтальное перемещение
			if (inputDevice.IsKeyDown(scheme.up))    comp->translate(DirectX::XMFLOAT3(0, speed * deltaTime, 0));
			if (inputDevice.IsKeyDown(scheme.down))  comp->translate(DirectX::XMFLOAT3(0, -speed * deltaTime, 0));
			if (inputDevice.IsKeyDown(scheme.left))  comp->translate(DirectX::XMFLOAT3(-speed * deltaTime, 0, 0));
			if (inputDevice.IsKeyDown(scheme.right)) comp->translate(DirectX::XMFLOAT3(speed * deltaTime, 0, 0));

			// Поворот
			if (inputDevice.IsKeyDown(Keys::Up))    comp->rotate(DirectX::XMFLOAT3(speed * deltaTime, 0, 0));
			if (inputDevice.IsKeyDown(Keys::Down))  comp->rotate(DirectX::XMFLOAT3(-speed * deltaTime, 0, 0));
			if (inputDevice.IsKeyDown(Keys::Left))  comp->rotate(DirectX::XMFLOAT3(0, speed * deltaTime, 0));
			if (inputDevice.IsKeyDown(Keys::Right)) comp->rotate(DirectX::XMFLOAT3(0, -speed * deltaTime, 0));
		}
	}
#pragma endregion
	inputDevice.UpdateKeyStates();
}
void Game::ballCheck() {
	if (ball != nullptr) {
		int ballScored = ball->isOutOfBounds();
		if (ballScored) {
			// Удаляем
			for (auto it = Components.begin(); it != Components.end(); ++it) {
				if (*it == ball) {
					Components.erase(it);
					break;
				}
			}
			delete ball;
			ball = nullptr;
			if (ballScored == 1) {
				player2Score++;
				std::wcout << L"Player 2 scored! Score: " << player1Score << " - " << player2Score << std::endl;
			}
			else if (ballScored == 2) {
				player1Score++;
				std::wcout << L"Player 1 scored! Score: " << player1Score << " - " << player2Score << std::endl;
			}
		}
	}
	if (ball == nullptr) {
		spawnBall();
	}
}
void Game::CreateDepthBuffer()
{
	// Всякие непонятные свойства текстуры для буфера глубины
	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = Display.clientWidth;           
	depthTexDesc.Height = Display.clientHeight;         
	depthTexDesc.MipLevels = 1;                         // хз
	depthTexDesc.ArraySize = 1;                         
	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //хз
	depthTexDesc.SampleDesc.Count = 1;                  // хз
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;           // хз
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;  // хз
	depthTexDesc.CPUAccessFlags = 0;                    // хз
	depthTexDesc.MiscFlags = 0;

	// тут даннык глубины
	HRESULT hr = Device->CreateTexture2D(&depthTexDesc, nullptr, &depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(Display.hwnd, L"Failed to create depth stencil texture", L"Error", MB_OK);
		return;
	}

	// ещё какие-то настроечки для буфера глубины
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
	depthViewDesc.Format = depthTexDesc.Format;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; 
	depthViewDesc.Texture2D.MipSlice = 0;

	// записать данные глубины в буфер глубины
	hr = Device->CreateDepthStencilView(depthStencilBuffer, &depthViewDesc, &depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(Display.hwnd, L"Failed to create depth stencil view", L"Error", MB_OK);
		depthStencilBuffer->Release();
		depthStencilBuffer = nullptr;
		return;
	}
}

void Game::BindDepthBuffer()
{
	context->OMSetRenderTargets(1, &rtv, depthStencilView);
}