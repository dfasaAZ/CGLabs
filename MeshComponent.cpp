#include "MeshComponent.h"
#include "Game.h"
#include <d3dcompiler.h>

MeshComponent::MeshComponent(Game* g, const DirectX::XMFLOAT4& color)
    : GameComponent(g)
    , meshColor(color)
    , stride(sizeof(VertexPositionColorNormal))
    , offset(0)
    , initialized(false)
{
    createConstantBuffer();
    initialize();
}

MeshComponent::~MeshComponent()
{
    destroyResources();
}

void MeshComponent::initialize()
{
    // Compile vertex shader
    ComPtr<ID3DBlob> vertexBlob;
    HRESULT hr = CompileShaderFromFile(
        L"./Shaders/Mesh3DShader.hlsl",
        nullptr,
        "VSMain",
        "vs_5_0",
        vertexBlob.GetAddressOf());

    if (FAILED(hr))
    {
        MessageBox(game->Display.hwnd, L"Failed to compile vertex shader", L"Error", MB_OK);
        return;
    }

    // Compile pixel shader
    ComPtr<ID3DBlob> pixelBlob;
    hr = CompileShaderFromFile(
        L"./Shaders/Mesh3DShader.hlsl",
        nullptr,
        "PSMain",
        "ps_5_0",
        pixelBlob.GetAddressOf());

    if (FAILED(hr))
    {
        MessageBox(game->Display.hwnd, L"Failed to compile pixel shader", L"Error", MB_OK);
        return;
    }

    // Create shaders
    game->Device->CreateVertexShader(
        vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(),
        nullptr,
        vertexShader.GetAddressOf());

    game->Device->CreatePixelShader(
        pixelBlob->GetBufferPointer(),
        pixelBlob->GetBufferSize(),
        nullptr,
        pixelShader.GetAddressOf());

    // Create input layout
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    game->Device->CreateInputLayout(
        inputElements,
        3,
        vertexBlob->GetBufferPointer(),
        vertexBlob->GetBufferSize(),
        inputLayout.GetAddressOf());

    // Create rasterizer state
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_FRONT;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthClipEnable = TRUE;

    game->Device->CreateRasterizerState(&rasterDesc, rasterizerState.GetAddressOf());

    // Create depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    game->Device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());

    initialized = true;
}

void MeshComponent::createBuffers(const std::vector<VertexPositionColorNormal>& vertices,
    const std::vector<uint32_t>& indices)
{
    meshVertices = vertices;
    meshIndices = indices;

    // Create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(VertexPositionColorNormal) * vertices.size());
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    game->Device->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer.GetAddressOf());

    // Create index buffer
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indices.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    game->Device->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer.GetAddressOf());
}

void MeshComponent::destroyResources()
{
    // COM pointers auto-release
}

void MeshComponent::draw()
{
    if (!initialized || meshVertices.empty())
        return;

    // Update constant buffer (already handled by base class)
    ID3D11Buffer* cb = getConstantBuffer();

    game->context->RSSetState(rasterizerState.Get());
    game->context->OMSetDepthStencilState(depthStencilState.Get(), 0);

    game->context->VSSetConstantBuffers(0, 1, &cb);
    game->context->PSSetConstantBuffers(0, 1, &cb);

    game->context->IASetInputLayout(inputLayout.Get());
    game->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    game->context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    game->context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

    game->context->VSSetShader(vertexShader.Get(), nullptr, 0);
    game->context->PSSetShader(pixelShader.Get(), nullptr, 0);

    game->context->DrawIndexed(static_cast<UINT>(meshIndices.size()), 0, 0);
}

void MeshComponent::update()
{
}
void MeshComponent::reload()
{
    destroyResources();
    initialize();
}
void MeshComponent::resolveCollision(GameComponent* other)
{
	GameComponent::resolveCollision(other);
}

void MeshComponent::setColor(const DirectX::XMFLOAT4& color)
{
    meshColor = color;

    // Update all vertices with new color
    for (auto& vertex : meshVertices)
    {
        vertex.color = color;
    }

    // Recreate vertex buffer
    if (!meshVertices.empty())
    {
        vertexBuffer.Reset();
        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(VertexPositionColorNormal) * meshVertices.size());
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexData = {};
        vertexData.pSysMem = meshVertices.data();

        game->Device->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer.GetAddressOf());
    }
}