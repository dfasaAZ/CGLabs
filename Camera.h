#pragma once
#include <directxmath.h>
#include <vector>

class GameComponent;

enum class CameraMode { Free, FirstPerson, ThirdPerson, Fixed };

class Camera {
public:
    Camera(float aspectRatio);
	~Camera();

    void Update(float deltaTime);
    void UpdateAspectRatio(float width, float height);

    // Matrix getters
    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;

    // Movement
    void MoveForward(float amount);
    void MoveRight(float amount);
    void MoveUp(float amount);
    void Yaw(float angle);
    void Pitch(float angle);

    // Mode management
    void SetMode(CameraMode mode, GameComponent* target = nullptr);
    void AttachToFP(GameComponent* target, float eyeHeight = 1.5f);
    void AttachToTP(GameComponent* target, float distance = 5.0f, float height = 2.0f);
    void SetFixedView(const DirectX::XMFLOAT3& position,const DirectX::XMFLOAT3& target);

    // Getters
    DirectX::XMFLOAT3 GetPosition() const { return position; }
    CameraMode GetMode() const { return mode; }

private:
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 target;
    DirectX::XMFLOAT3 up;
    float fov;
    float nearPlane, farPlane;
    float aspectRatio;

    CameraMode mode;
    GameComponent* followTarget;
    float followDistance;
    float followHeight;
    float eyeOffset;

    float smoothFactor;
    DirectX::XMFLOAT3 smoothPosition;
    DirectX::XMFLOAT3 smoothTarget;
};
