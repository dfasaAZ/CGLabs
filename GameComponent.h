#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
class Game;

class GameComponent
{
public:
	Game* game;
	GameComponent(Game* g);
	virtual ~GameComponent();
	virtual void destroyResources() = 0;
	virtual void draw() = 0;
	virtual void initialize() = 0;
	virtual void reload() = 0;
	virtual void update() = 0;

	void setPosition(DirectX::XMFLOAT3 position);
	void setRotation(DirectX::XMFLOAT3 rotation);
	void setScale(DirectX::XMFLOAT3 scale);

	void setPosition(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setScale(float x, float y, float z);

	DirectX::XMFLOAT3 getPosition() const;
	DirectX::XMFLOAT3 getRotation() const;
	DirectX::XMFLOAT3 getScale() const;

	void translate(DirectX::XMFLOAT3 delta);
	void rotate(DirectX::XMFLOAT3 delta);

protected:
	ID3D11Buffer* getConstantBuffer() { return constantBuffer; }
	void updateConstantBuffer();
	void createConstantBuffer();

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	ID3D11Buffer* constantBuffer;

	struct ConstantBuffer {
		DirectX::XMMATRIX worldViewProj;
		DirectX::XMMATRIX world;
	};
};