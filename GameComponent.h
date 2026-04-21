#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "PhysicsComponent.h"
#include <vector>

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
	virtual void update(float deltaTime) = 0;

	void setPosition(DirectX::XMFLOAT3 position);
	void setRotation(DirectX::XMFLOAT3 rotation);
	void setScale(DirectX::XMFLOAT3 scale);

	void setPosition(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setScale(float x, float y, float z);

	DirectX::XMFLOAT3 getPosition() const;
	DirectX::XMFLOAT3 getRotation() const;
	virtual DirectX::XMFLOAT3 getScale() const;

	void translate(DirectX::XMFLOAT3 delta);
	void rotate(DirectX::XMFLOAT3 delta);

	//Вращение на кватернионах(не использую)
	void setRotation(const DirectX::XMFLOAT4& quat);
	void setRotationEuler(const DirectX::XMFLOAT3& euler);
	DirectX::XMFLOAT4 getRotationQuat() const;
	void rotate(const DirectX::XMFLOAT4& deltaQuat);
	void rotateEuler(const DirectX::XMFLOAT3& deltaEuler);

	void setPositionConstraint(float x, float y, float z);
	DirectX::XMFLOAT3 getPositionConstraint() const;

	void setRotationConstraint(float x, float y, float z);
	DirectX::XMFLOAT3 getRotationConstraint() const;

	void setScaleConstraint(float x, float y, float z);
	DirectX::XMFLOAT3 getScaleConstraint() const;

	//Добавить компонент наследник
	virtual void addChild(GameComponent* parent);

	// Physics
	PhysicsComponent* getPhysics();
	void updatePhysics(float deltaTime);
	bool checkCollision(GameComponent* other);
	virtual void resolveCollision(GameComponent* other)=0;

protected:
	ID3D11Buffer* getConstantBuffer() { return constantBuffer; }
	void updateConstantBuffer();
	void createConstantBuffer();

	// Create buffers from provided memory. Return created buffer or nullptr on failure.
	ID3D11Buffer* createVertexBuffer(const void* data, UINT byteWidth);
	ID3D11Buffer* createIndexBuffer(const void* data, UINT byteWidth);

	static HRESULT CompileShaderFromFile(LPCWSTR filePath, const D3D_SHADER_MACRO* macros, LPCSTR entryPoint, LPCSTR target, ID3DBlob** outBlob);

protected:
	PhysicsComponent* physics;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT3 scale;

	//Ограничитель перемещения по осям (1 - разрешено, 0 - запрещено)
	DirectX::XMFLOAT3 positonConstraint;
	//Ограничитель вращения по осям (1 - разрешено, 0 - запрещено)
	DirectX::XMFLOAT3 rotationConstraint;
	//Ограничитель масштабирования по осям (1 - разрешено, 0 - запрещено)
	DirectX::XMFLOAT3 scaleConstraint;

	//Компоненты наследники
	std::vector<GameComponent*> childComponents;

	ID3D11Buffer* constantBuffer = nullptr;

	// На всякий случай
	ID3D11Buffer* vb = nullptr;
	ID3D11Buffer* ib = nullptr;

	struct ConstantBuffer {
		DirectX::XMMATRIX worldViewProj;
		DirectX::XMMATRIX world;
	};
};