#include "RectangleComponent.h"
#include "Game.h"
#include <d3dcompiler.h>
#include <iostream>
//Конструктор с передачей точек
RectangleComponent::RectangleComponent(Game* g, DirectX::XMFLOAT4 points[8])
	: GameComponent(g)
	, rastState(nullptr)
	, vertexShader(nullptr)
	, pixelShader(nullptr)
	, layout(nullptr)
	, vb(nullptr)
	, ib(nullptr)
{
	for (int i = 0; i < 8; ++i) {
		this->points[i] = points[i];
	}
	pointsCount = 8;
	createConstantBuffer();
	this->initialize();
}
//Коестпкор с дефолтными точками
RectangleComponent::RectangleComponent(Game* g, DirectX::XMFLOAT4 color)  : GameComponent(g)
	, rastState(nullptr)
	, vertexShader(nullptr)
	, pixelShader(nullptr)
	, layout(nullptr)
	, vb(nullptr)
	, ib(nullptr)
{
	DirectX::XMFLOAT4 rectPoints[8] = {
	DirectX::XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f), color,   // Bottom-left 
	DirectX::XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f),  	color,   // Bottom-right 
	DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f),   	color,   // Top-right 
	DirectX::XMFLOAT4(-0.5f, 0.5f, 0.0f, 1.0f),  	color    // Top-left 
	};
	for (int i = 0; i < 8; ++i) {
		this->points[i] = rectPoints[i];
	}
	pointsCount = 8;
	createConstantBuffer();
	this->initialize();
}

void RectangleComponent::initialize() {
	ID3DBlob* vertexBC = nullptr;
	auto res = CompileShaderFromFile(L"./Shaders/BlueRectangleShader.hlsl",
		nullptr /*macros*/,
		"VSMain",
		"vs_5_0",
		&vertexBC);

	if (FAILED(res)) {
		MessageBox(game->Display.hwnd, L"BlueRectangleShader.hlsl", L"Missing Shader File or compile error", MB_OK);
		return;
	}

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};

	ID3DBlob* pixelBC = nullptr;
	res = CompileShaderFromFile(L"./Shaders/BlueRectangleShader.hlsl",
		nullptr,
		"PSMain",
		"ps_5_0",
		&pixelBC);

	if (FAILED(res)) {
		if (vertexBC) vertexBC->Release();
		MessageBox(game->Display.hwnd, L"BlueRectangleShader.hlsl", L"Missing Shader File or compile error", MB_OK);
		return;
	}

	// Create shaders
	game->Device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &vertexShader);

	game->Device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &pixelShader);

	game->Device->CreateInputLayout(
		inputElements,
		2,
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout);

	vb = createVertexBuffer(points, static_cast<UINT>(sizeof(DirectX::XMFLOAT4) * 8)); // sizeof(points) = 
	int indeces[] = { 0, 1, 2, 2, 3, 0 };
	ib = createIndexBuffer(indeces, static_cast<UINT>(sizeof(indeces)));

	strides[0] = 32u;
	offsets[0] = 0u;

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = game->Device->CreateRasterizerState(&rastDesc, &rastState);

	if (vertexBC) vertexBC->Release();
	if (pixelBC) pixelBC->Release();
	updateConstantBuffer();
}

void RectangleComponent::draw() {
	ID3D11Buffer* cb = getConstantBuffer();
	game->context->RSSetState(rastState);
	game->context->VSSetConstantBuffers(0, 1, &cb);

	game->context->IASetInputLayout(layout);
	game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	game->context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	game->context->IASetVertexBuffers(0, 1, &vb, strides, offsets);

	game->context->VSSetShader(vertexShader, nullptr, 0);
	game->context->PSSetShader(pixelShader, nullptr, 0);

	game->context->DrawIndexed(6, 0, 0);
}

void RectangleComponent::update() {

}

void RectangleComponent::destroyResources() {
	if (rastState) { rastState->Release(); rastState = nullptr; }
	if (vertexShader) { vertexShader->Release(); vertexShader = nullptr; }
	if (pixelShader) { pixelShader->Release(); pixelShader = nullptr; }
	if (layout) { layout->Release(); layout = nullptr; }
	if (vb) { vb->Release(); vb = nullptr; }
	if (ib) { ib->Release(); ib = nullptr; }
}

void RectangleComponent::reload() {
	destroyResources();
	initialize();
}

RectangleComponent::~RectangleComponent() {
	destroyResources();
}