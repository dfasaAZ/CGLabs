#pragma once
#include "GameComponent.h"

class RectangleComponent : public GameComponent
{
public:
	RectangleComponent(Game* game, DirectX::XMFLOAT4 points[8]);
	RectangleComponent(Game* game, DirectX::XMFLOAT4 color);
	~RectangleComponent();
	void initialize() override;
	void update(float deltaTime) override;
	void draw() override;
	void destroyResources() override;
	void reload() override;
	void resolveCollision(GameComponent* other) override;

private:
	ID3D11RasterizerState* rastState;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	ID3D11InputLayout* layout;

	DirectX::XMFLOAT4 points[8];
	int pointsCount;

	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	UINT strides[1];
	UINT offsets[1];
};