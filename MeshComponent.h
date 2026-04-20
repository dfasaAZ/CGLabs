#pragma once
#include "GameComponent.h"
#include "VertexTypes.h"
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class MeshComponent : public GameComponent
{
public:
    MeshComponent(Game* g, const DirectX::XMFLOAT4& color);
    virtual ~MeshComponent();

    virtual void draw() override;
    virtual void update(float deltaTime) override;
    void initialize() override;
    void destroyResources() override;
    void reload() override;
    void resolveCollision(GameComponent* other) override;

    void setColor(const DirectX::XMFLOAT4& color);
    DirectX::XMFLOAT4 getColor() const { return meshColor; }

protected:
    virtual void generateMesh() = 0;
    void createBuffers(const std::vector<VertexPositionColorNormal>& vertices,
        const std::vector<uint32_t>& indices);

    DirectX::XMFLOAT4 meshColor;
    std::vector<VertexPositionColorNormal> meshVertices;
    std::vector<uint32_t> meshIndices;

private:

    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    ComPtr<ID3D11RasterizerState> rasterizerState;
    ComPtr<ID3D11DepthStencilState> depthStencilState;

    UINT stride;
    UINT offset;
    bool initialized;
};