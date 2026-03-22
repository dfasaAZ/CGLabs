#pragma once
#include "GameComponent.h"
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
class TriangleComponent :public GameComponent
{
	public:

	TriangleComponent(Game* game, DirectX::XMFLOAT4 points[6]);
	~TriangleComponent();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DestroyResources() override;
	void Reload() override;
private:
	ID3D11RasterizerState* rastState;
    ID3D11VertexShader* vertexShader ;
    ID3D11PixelShader* pixelShader ;

    ID3D11InputLayout* layout ;

	DirectX::XMFLOAT4 points[6];

    ID3D11Buffer* vb ;
    ID3D11Buffer* ib ;
	UINT strides[1];
	UINT offsets[1];

};

