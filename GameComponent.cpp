#include "GameComponent.h"
#include "Game.h"

GameComponent::GameComponent(Game* g) : game(g), 
position(0.0f, 0.0f, 0.0f), 
rotation(0.0f, 0.0f, 0.0f), 
scale(1.0f, 1.0f, 1.0f), 
constantBuffer(nullptr)
{
}

GameComponent::~GameComponent()
{
    if (constantBuffer) {
        constantBuffer->Release();
        constantBuffer = nullptr;
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
void GameComponent::translate(DirectX::XMFLOAT3 delta) {
    setPosition(DirectX::XMFLOAT3(position.x + delta.x, position.y + delta.y, position.z + delta.z));
	printf("Position: (%f, %f, %f)\n", position.x, position.y, position.z);
}
void GameComponent::rotate(DirectX::XMFLOAT3 delta) {
    setRotation(DirectX::XMFLOAT3(rotation.x + delta.x, rotation.y + delta.y, rotation.z + delta.z));
}
void GameComponent::updateConstantBuffer() {
    if (!constantBuffer) return;
    ConstantBuffer cb;
    DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
                             DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
                             DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    // Transpose matrices before sending to HLSL (HLSL expects column-major by default)
    cb.world = DirectX::XMMatrixTranspose(world);

    float aspect = static_cast<float>(game->Display.clientWidth) /
        static_cast<float>(game->Display.clientHeight);
    DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(-aspect, aspect, -1.0f, 1.0f, 0.1f, 100.0f);
    DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
    cb.worldViewProj = DirectX::XMMatrixTranspose(world * view * proj);//До конца не понял что это

    game->context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
}
void GameComponent::createConstantBuffer() {
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    game->Device->CreateBuffer(&cbd, nullptr, &constantBuffer);
}