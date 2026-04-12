#include "GameComponent.h"
#include "Game.h"
#include <iostream>

GameComponent::GameComponent(Game* g) : game(g),
	position(0.0f, 0.0f, 0.0f),
	rotation(0.0f, 0.0f, 0.0f),
	scale(1.0f, 1.0f, 1.0f),
	constantBuffer(nullptr),
	vb(nullptr),
	ib(nullptr),
	positonConstraint(1.0f, 1.0f, 1.0f),
	rotationConstraint(1.0f, 1.0f, 1.0f),
	scaleConstraint(1.0f, 1.0f, 1.0f),
	physics(nullptr)
{
	physics = new PhysicsComponent();
}

GameComponent::~GameComponent()
{
	if (constantBuffer) {
		constantBuffer->Release();
		constantBuffer = nullptr;
	}
	if (vb) { vb->Release(); vb = nullptr; }
	if (ib) { ib->Release(); ib = nullptr; }
	if (physics) {
		delete physics;
		physics = nullptr;
	}
}

void GameComponent::setPosition(DirectX::XMFLOAT3 position) {
	this->position = position;
	updateConstantBuffer();
}

void GameComponent::setRotation(DirectX::XMFLOAT3 rotation) {
	this->rotation = rotation;
	updateConstantBuffer();
}

void GameComponent::setScale(DirectX::XMFLOAT3 scale) {
	this->scale = scale;
	updateConstantBuffer();
}

void GameComponent::setPosition(float x, float y, float z) {
	setPosition(DirectX::XMFLOAT3(x, y, z));
}

void GameComponent::setRotation(float x, float y, float z) {
	setRotation(DirectX::XMFLOAT3(x, y, z));
}

void GameComponent::setScale(float x, float y, float z) {
	setScale(DirectX::XMFLOAT3(x, y, z));
}

DirectX::XMFLOAT3 GameComponent::getPosition() const {
	return position;
}

DirectX::XMFLOAT3 GameComponent::getRotation() const {
	return rotation;
}

DirectX::XMFLOAT3 GameComponent::getScale() const {
	return scale;
}
PhysicsComponent* GameComponent::getPhysics() {
	return physics;
}

void GameComponent::translate(DirectX::XMFLOAT3 delta) {
	setPosition(DirectX::XMFLOAT3(
		position.x + (delta.x * positonConstraint.x),
		position.y + (delta.y * positonConstraint.y),
		position.z + (delta.z * positonConstraint.z)));
	//printf("Position: (%f, %f, %f)\n", position.x, position.y, position.z);
}

void GameComponent::rotate(DirectX::XMFLOAT3 delta) {
	setRotation(DirectX::XMFLOAT3(
		rotation.x + (delta.x * rotationConstraint.x),
		rotation.y + (delta.y * rotationConstraint.y),
		rotation.z + (delta.z * rotationConstraint.z)));
}

void GameComponent::setPositionConstraint(float x, float y, float z) {
	positonConstraint = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 GameComponent::getPositionConstraint() const {
	return positonConstraint;
}

void GameComponent::setRotationConstraint(float x, float y, float z) {
	rotationConstraint = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 GameComponent::getRotationConstraint() const {
	return rotationConstraint;
}

void GameComponent::setScaleConstraint(float x, float y, float z) {
	scaleConstraint = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 GameComponent::getScaleConstraint() const {
	return scaleConstraint;
}
void GameComponent::update() {
}
void GameComponent::updatePhysics(float deltaTime) {
	if (physics) {
		physics->update(deltaTime, this);
	}
}

bool GameComponent::checkCollision(GameComponent* other) {
	if (!physics || !other || !other->physics) return false;
	return physics->checkAABBCollision(
		other->physics,
		position, scale,
		other->position, other->scale);
}

void GameComponent::resolveCollision(GameComponent* other) {
	if (!physics || !other || !other->physics) return;
	physics->resolveAABBCollision(
		other->physics,
		this,
		other);
	updateConstantBuffer();
	other->updateConstantBuffer();
}

void GameComponent::updateConstantBuffer() {
	if (!constantBuffer) return;
	ConstantBuffer cb;
	DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
		DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	cb.world = DirectX::XMMatrixTranspose(world);

	float aspect = static_cast<float>(game->Display.clientWidth) /
		static_cast<float>(game->Display.clientHeight);

	// Perspective projection instead of orthographic
	DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(60.0f),  // 60° field of view
		aspect,                               // Aspect ratio
		0.1f,                                 // Near plane
		100.0f);                              // Far plane

	// Camera positioned at (0, 0, -5) looking at origin
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, at, up);

	cb.worldViewProj = DirectX::XMMatrixTranspose(world * view * proj);

	game->context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
}

void GameComponent::createConstantBuffer() {
	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	game->Device->CreateBuffer(&cbd, nullptr, &constantBuffer);
}

HRESULT GameComponent::CompileShaderFromFile(
	LPCWSTR filePath,
	const D3D_SHADER_MACRO* macros,
	LPCSTR entryPoint,
	LPCSTR target,
	ID3DBlob** outBlob)
{
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		filePath,
		macros,
		nullptr,
		entryPoint,
		target,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shaderBlob,
		&errorBlob);

	if (FAILED(hr)) {
		if (errorBlob) {
			char* compileErrors = static_cast<char*>(errorBlob->GetBufferPointer());
			std::cout << compileErrors << std::endl;
			errorBlob->Release();
		}
		if (shaderBlob) shaderBlob->Release();
		return hr;
	}

	*outBlob = shaderBlob;
	return S_OK;
}

ID3D11Buffer* GameComponent::createVertexBuffer(const void* data, UINT byteWidth) {
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = byteWidth;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = data;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer = nullptr;
	if (SUCCEEDED(game->Device->CreateBuffer(&vertexBufDesc, &vertexData, &buffer))) {
		return buffer;
	}
	return nullptr;
}

ID3D11Buffer* GameComponent::createIndexBuffer(const void* data, UINT byteWidth) {
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = byteWidth;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = data;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer = nullptr;
	if (SUCCEEDED(game->Device->CreateBuffer(&indexBufDesc, &indexData, &buffer))) {
		return buffer;
	}
	return nullptr;
}