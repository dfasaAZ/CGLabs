#include "TriangleComponent.h"
#include "Game.h"
#include <d3dcompiler.h>
#include <iostream>

TriangleComponent::TriangleComponent(Game* g, DirectX::XMFLOAT4 points[6])
	: GameComponent(g)
	, rastState(nullptr)
	, vertexShader(nullptr)
	, pixelShader(nullptr)
	, layout(nullptr)
	, vb(nullptr)
	, ib(nullptr)
{
    for (int i = 0; i < 6; ++i) {
        this->points[i] = points[i];
    }
	pointsCount = 6;
	createConstantBuffer();
	this->initialize();
}

void TriangleComponent::initialize() {
	ID3DBlob* vertexBC = nullptr;
	auto res = CompileShaderFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
		nullptr /*macros*/,
		"VSMain",
		"vs_5_0",
		&vertexBC);

	if (FAILED(res)) {
		MessageBox(game->Display.hwnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File or compile error", MB_OK);
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

	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	ID3DBlob* pixelBC = nullptr;
	res = CompileShaderFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
		Shader_Macros,
		"PSMain",
		"ps_5_0",
		&pixelBC);

	if (FAILED(res)) {
		if (vertexBC) vertexBC->Release();
		MessageBox(game->Display.hwnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File or compile error", MB_OK);
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

	// create vertex/index buffers from explicit memory
	vb = createVertexBuffer(points, static_cast<UINT>(sizeof(points))); // sizeof(points) = sizeof(XMFLOAT4)*6
	int indeces[] = {0,1,2};
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

void TriangleComponent::draw() {
	ID3D11Buffer* cb = getConstantBuffer();
	game->context->RSSetState(rastState);
	game->context->VSSetConstantBuffers(0, 1, &cb);

	game->context->IASetInputLayout(layout);
	game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	game->context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	game->context->IASetVertexBuffers(0, 1, &vb, strides, offsets);

	game->context->VSSetShader(vertexShader, nullptr, 0);
	game->context->PSSetShader(pixelShader, nullptr, 0);

	game->context->DrawIndexed(3, 0, 0);
}
void TriangleComponent::update(float deltaTime) {

}

void TriangleComponent::destroyResources() {
	if (rastState) { rastState->Release(); rastState = nullptr; }
	if (vertexShader) { vertexShader->Release(); vertexShader = nullptr; }
	if (pixelShader) { pixelShader->Release(); pixelShader = nullptr; }
	if (layout) { layout->Release(); layout = nullptr; }
	if (vb) { vb->Release(); vb = nullptr; }
	if (ib) { ib->Release(); ib = nullptr; }
}

void TriangleComponent::reload() {
	destroyResources();
	initialize();
}

TriangleComponent::~TriangleComponent() {
	destroyResources();
}