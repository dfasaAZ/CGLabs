#pragma once
#include "GameComponent.h"

class TriangleComponent :public GameComponent
{
	public:

	TriangleComponent(Game* game, DirectX::XMFLOAT4 points[6]);
	~TriangleComponent();
	void initialize() override;
	void update(float deltaTime) override;
	void draw() override;
	void destroyResources() override;
	void reload() override;

private:
	ID3D11RasterizerState* rastState;
    ID3D11VertexShader* vertexShader ;
    ID3D11PixelShader* pixelShader ;

    ID3D11InputLayout* layout ;

	DirectX::XMFLOAT4 points[6];
	int pointsCount;

    ID3D11Buffer* vb ;
    ID3D11Buffer* ib ;
	UINT strides[1];
	UINT offsets[1];

};

