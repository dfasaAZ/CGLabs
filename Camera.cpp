#include "Camera.h"
#include "GameComponent.h"  // Include full header, not just forward declaration
#include <cmath>

Camera::Camera(float aspectRatio)
    : position(0.0f, 0.0f, -5.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , fov(DirectX::XMConvertToRadians(60.0f))
    , nearPlane(0.1f)
    , farPlane(100.0f)
    , aspectRatio(aspectRatio)
    , mode(CameraMode::Free)
    , followTarget(nullptr)
    , followDistance(5.0f)
    , followHeight(2.0f)
    , eyeOffset(1.5f)
    , smoothFactor(0.1f)
    , smoothPosition(position)
    , smoothTarget(target)
{
}

Camera::~Camera() {
}

void Camera::UpdateAspectRatio(float width, float height) {
    aspectRatio = width / height;
}

DirectX::XMMATRIX Camera::GetViewMatrix() const {
    DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&smoothPosition);
    DirectX::XMVECTOR at = DirectX::XMLoadFloat3(&smoothTarget);
    DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
    return DirectX::XMMatrixLookAtLH(eye, at, upVec);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix() const {
    return DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::MoveForward(float amount) {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR tgt = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR forward = DirectX::XMVectorSubtract(tgt, pos);
    forward = DirectX::XMVector3Normalize(forward);

    DirectX::XMVECTOR delta = DirectX::XMVectorScale(forward, amount);
    pos = DirectX::XMVectorAdd(pos, delta);
    tgt = DirectX::XMVectorAdd(tgt, delta);

    DirectX::XMStoreFloat3(&position, pos);
    DirectX::XMStoreFloat3(&target, tgt);
}

void Camera::MoveRight(float amount) {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR tgt = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

    DirectX::XMVECTOR forward = DirectX::XMVectorSubtract(tgt, pos);
    forward = DirectX::XMVector3Normalize(forward);
    DirectX::XMVECTOR right = DirectX::XMVector3Cross(upVec, forward);
    right = DirectX::XMVector3Normalize(right);

    DirectX::XMVECTOR delta = DirectX::XMVectorScale(right, amount);
    pos = DirectX::XMVectorAdd(pos, delta);
    tgt = DirectX::XMVectorAdd(tgt, delta);

    DirectX::XMStoreFloat3(&position, pos);
    DirectX::XMStoreFloat3(&target, tgt);
}

void Camera::MoveUp(float amount) {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR tgt = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

    DirectX::XMVECTOR delta = DirectX::XMVectorScale(upVec, amount);
    pos = DirectX::XMVectorAdd(pos, delta);
    tgt = DirectX::XMVectorAdd(tgt, delta);

    DirectX::XMStoreFloat3(&position, pos);
    DirectX::XMStoreFloat3(&target, tgt);
}

void Camera::Yaw(float angle) {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR tgt = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

    DirectX::XMVECTOR forward = DirectX::XMVectorSubtract(tgt, pos);
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(angle);

    forward = DirectX::XMVector3Transform(forward, rotation);
    tgt = DirectX::XMVectorAdd(pos, forward);

    DirectX::XMStoreFloat3(&target, tgt);
}

void Camera::Pitch(float angle) {
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR tgt = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

    DirectX::XMVECTOR forward = DirectX::XMVectorSubtract(tgt, pos);
    DirectX::XMVECTOR right = DirectX::XMVector3Cross(forward, upVec);
    right = DirectX::XMVector3Normalize(right);

    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(right, angle);
    forward = DirectX::XMVector3Transform(forward, rotation);

    // Clamp pitch to prevent camera flip
    DirectX::XMVECTOR newForward = DirectX::XMVector3Normalize(forward);
    DirectX::XMVECTOR newUp = DirectX::XMVector3Cross(right, newForward);
    float upY = DirectX::XMVectorGetY(newUp);

    if (upY < 0.1f && upY > -0.1f) {
        return; // Prevent over-rotation
    }

    tgt = DirectX::XMVectorAdd(pos, forward);
    DirectX::XMStoreFloat3(&target, tgt);
}

void Camera::SetMode(CameraMode newMode, GameComponent* target) {
	printf("Switching camera mode to %d\n", static_cast<int>(newMode));
    mode = newMode;
    followTarget = target;

    if (mode == CameraMode::Fixed) {
        smoothPosition = position;
        smoothTarget = target->getPosition();
    }
}

void Camera::AttachToFP(GameComponent* target, float eyeHeight) {
    SetMode(CameraMode::FirstPerson,target);
    followTarget = target;
    eyeOffset = eyeHeight;
}

void Camera::AttachToTP(GameComponent* target, float distance, float height) {
    SetMode(CameraMode::ThirdPerson,target);
    followDistance = distance;
    followHeight = height;
}

void Camera::SetFixedView(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& tgt) {
	SetMode(CameraMode::Fixed, nullptr);
    position = pos;
    target = tgt;
    smoothPosition = pos;
    smoothTarget = tgt;
}

void Camera::Update(float deltaTime) {
    if (mode == CameraMode::FirstPerson && followTarget) {
        DirectX::XMFLOAT3 targetPos = followTarget->getPosition();

        // Position at target's eye level
        position.x = targetPos.x;
        position.y = targetPos.y + eyeOffset;
        position.z = targetPos.z;

        // Get forward direction from target's rotation
        DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        DirectX::XMFLOAT4 quat = followTarget->getRotationQuat();
        DirectX::XMVECTOR rotQuat = DirectX::XMLoadFloat4(&quat);
        forward = DirectX::XMVector3Rotate(forward, rotQuat);

        // Set target
        DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR tgtVec = DirectX::XMVectorAdd(posVec, forward);
        DirectX::XMStoreFloat3(&target, tgtVec);
    }
    else if (mode == CameraMode::ThirdPerson && followTarget) {
        DirectX::XMFLOAT3 targetPos = followTarget->getPosition();
        DirectX::XMFLOAT4 quat = followTarget->getRotationQuat();
        DirectX::XMVECTOR targetRot = DirectX::XMLoadFloat4(&quat);

        // Get backward direction
        DirectX::XMVECTOR backward = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
        backward = DirectX::XMVector3Rotate(backward, targetRot);

        // Calculate desired camera position
        DirectX::XMVECTOR tgtPosVec = DirectX::XMLoadFloat3(&targetPos);
        DirectX::XMVECTOR offset = DirectX::XMVectorScale(backward, followDistance);
        DirectX::XMVECTOR upOffset = DirectX::XMVectorSet(0.0f, followHeight, 0.0f, 0.0f);

        DirectX::XMVECTOR desiredPos = DirectX::XMVectorAdd(tgtPosVec, offset);
        desiredPos = DirectX::XMVectorAdd(desiredPos, upOffset);

        // Smooth interpolation
        float t = 1.0f - powf(smoothFactor, deltaTime * 60.0f);
        DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR newPos = DirectX::XMVectorLerp(currentPos, desiredPos, t);

        DirectX::XMStoreFloat3(&position, newPos);
        target = targetPos;
    }

    // Apply smoothing to all modes
    float t = 1.0f - powf(0.1f, deltaTime * 60.0f);
    DirectX::XMVECTOR smoothPos = DirectX::XMLoadFloat3(&smoothPosition);
    DirectX::XMVECTOR smoothTgt = DirectX::XMLoadFloat3(&smoothTarget);
    DirectX::XMVECTOR actualPos = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR actualTgt = DirectX::XMLoadFloat3(&target);

    smoothPos = DirectX::XMVectorLerp(smoothPos, actualPos, t);
    smoothTgt = DirectX::XMVectorLerp(smoothTgt, actualTgt, t);

    DirectX::XMStoreFloat3(&smoothPosition, smoothPos);
    DirectX::XMStoreFloat3(&smoothTarget, smoothTgt);
}