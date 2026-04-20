#pragma once
#include "GameComponent.h"
#include "MeshComponent.h"
#include <DirectXMath.h>

class Planet : public GameComponent
{
private:
    float orbitSpeed;           // Speed of revolving around point (radians per second)
    float rotationSpeed;        // Speed of rotating around own axis (radians per second)
    MeshComponent* mesh;        // The visual mesh component
    float currentOrbitAngle;    // Current angle for orbit (radians)
    float currentRotationAngle; // Current angle for self-rotation (radians)
    DirectX::XMFLOAT3 orbitCenter; // Point to orbit around

public:
    Planet(Game* g, MeshComponent* meshComponent,
        float orbitSpeed = 1.0f,
        float rotationSpeed = 1.0f);

    virtual ~Planet();

    virtual void draw() override;
    void initialize() override;
    void destroyResources() override;
    void reload() override;
    void resolveCollision(GameComponent* other) override;
    virtual void addChild(GameComponent* child) override;

    // Orbit around the orbit center point
    void rotateAgainstParent(float deltaTime);

    // Rotate the mesh around its own axis
    void revolve(float deltaTime);

    // Update called each frame
    virtual void update(float deltaTime) override;

    // Getters and setters
    void setOrbitSpeed(float speed) { orbitSpeed = speed; }
    float getOrbitSpeed() const { return orbitSpeed; }

    void setRotationSpeed(float speed) { rotationSpeed = speed; }
    float getRotationSpeed() const { return rotationSpeed; }

    void setOrbitCenter(const DirectX::XMFLOAT3& center) { orbitCenter = center; }
    DirectX::XMFLOAT3 getOrbitCenter() const { return orbitCenter; }

    void setCurrentOrbitAngle(float angle) { currentOrbitAngle = angle; }
    float getCurrentOrbitAngle() const { return currentOrbitAngle; }

    void setCurrentRotationAngle(float angle) { currentRotationAngle = angle; }
    float getCurrentRotationAngle() const { return currentRotationAngle; }

    MeshComponent* getMesh() const { return mesh; }
};