#include "TriangleComponent.h"
#include "Game.h" // ensure Game is fully defined in this translation unit
#include <d3dcompiler.h>
#include <iostream>

TriangleComponent::TriangleComponent(Game* g)
	: GameComponent(g)
	, rastState(nullptr)
	, vertexShader(nullptr)
	, pixelShader(nullptr)
	, layout(nullptr)
	, vb(nullptr)
	, ib(nullptr)
{
}

void TriangleComponent::Initialize() {
	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;
	auto res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
		nullptr /*macros*/,
		nullptr /*include*/,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,
		&errorVertexCode);

	if (FAILED(res)) {
		// If the shader failed to compile it should have written something to the error message.
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
			errorVertexCode->Release();
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(game->Display.hwnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		if (vertexBC) vertexBC->Release();
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
	game->Device->CreateInputLayout(
		inputElements,
		2,
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout);
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	ID3DBlob* pixelBC = nullptr;
	ID3DBlob* errorPixelCode = nullptr;
	res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl", Shader_Macros /*macros*/, nullptr /*include*/, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);

	if (FAILED(res)) {
		if (errorPixelCode) {
			char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
			errorPixelCode->Release();
		}
		else {
			MessageBox(game->Display.hwnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}
		if (vertexBC) vertexBC->Release();
		if (pixelBC) pixelBC->Release();
		return;
	}

	game->Device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &vertexShader);

	game->Device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &pixelShader);

	// use the member `layout` (do not redeclare a new local variable that shadows it)
	game->Device->CreateInputLayout(
		inputElements,
		2,
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout);

	DirectX::XMFLOAT4 points[8] = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};


	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	// avoid size_t -> UINT conversion warning by casting explicitly
	vertexBufDesc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT4) * std::size(points));

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	game->Device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

	int indeces[] = { 0,1,2, 1,0,3 };
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	// avoid size_t -> UINT conversion warning by casting explicitly
	indexBufDesc.ByteWidth = static_cast<UINT>(sizeof(int) * std::size(indeces));

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;


	game->Device->CreateBuffer(&indexBufDesc, &indexData, &ib);

	// assign to index 0 (arrays have size 1)
	strides[0] = 32u;
	offsets[0] = 0u;



	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;


	res = game->Device->CreateRasterizerState(&rastDesc, &rastState);

	if (vertexBC) vertexBC->Release();
	if (pixelBC) pixelBC->Release();
}

void TriangleComponent::Draw() {
	// RSSetState
	game->context->RSSetState(rastState);

	// Viewport
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(game->Display.clientWidth);
	viewport.Height = static_cast<float>(game->Display.clientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;
	game->context->RSSetViewports(1, &viewport);

	// Input assembler
	game->context->IASetInputLayout(layout);
	game->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	game->context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	game->context->IASetVertexBuffers(0, 1, &vb, strides, offsets);

	// Shaders
	game->context->VSSetShader(vertexShader, nullptr, 0);
	game->context->PSSetShader(pixelShader, nullptr, 0);

	// Draw
	game->context->DrawIndexed(6, 0, 0);
}
void TriangleComponent::Update() {
	// No update logic for this simple triangle
}

void TriangleComponent::DestroyResources() {
	if (rastState) { rastState->Release(); rastState = nullptr; }
	if (vertexShader) { vertexShader->Release(); vertexShader = nullptr; }
	if (pixelShader) { pixelShader->Release(); pixelShader = nullptr; }
	if (layout) { layout->Release(); layout = nullptr; }
	if (vb) { vb->Release(); vb = nullptr; }
	if (ib) { ib->Release(); ib = nullptr; }
}

void TriangleComponent::Reload() {
	DestroyResources();
	Initialize();
}

TriangleComponent::~TriangleComponent() {
	DestroyResources();
}