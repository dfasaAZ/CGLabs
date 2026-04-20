#include "Planet.h"
#include "Game.h"
#include <cmath>

Planet::Planet(Game* g, MeshComponent* meshComponent,
    float orbitSpeed, float rotationSpeed)
    : GameComponent(g)
    , mesh(meshComponent)
    , orbitSpeed(orbitSpeed)
    , rotationSpeed(rotationSpeed)
    , currentOrbitAngle(0.0f)
    , currentRotationAngle(0.0f)
    , orbitCenter(0.0f, 0.0f, 0.0f)
{
    if (mesh)
    {
        orbitCenter = this->getPosition();
        addChild(mesh);
    }
    createConstantBuffer();
}

Planet::~Planet()
{
    mesh = nullptr;
}

void Planet::addChild(GameComponent* child)
{
   
    GameComponent::addChild(child);

    if (child)
    {
        // Спутники
        Planet* planetChild = dynamic_cast<Planet*>(child);
        if (planetChild)
        {
            planetChild->setOrbitCenter(this->position);
        }
    }
}

void Planet::rotateAgainstParent(float deltaTime)
{
    currentOrbitAngle += orbitSpeed * deltaTime;

    // костыль от жпт
    if (currentOrbitAngle > 2.0f * 3.14159265359f)
    {
        currentOrbitAngle -= 2.0f * 3.14159265359f;
    }

    // Calculate new position on the orbital circle
    float radius = sqrtf(
        (position.x - orbitCenter.x) * (position.x - orbitCenter.x) +
        (position.y - orbitCenter.y) * (position.y - orbitCenter.y) +
        (position.z - orbitCenter.z) * (position.z - orbitCenter.z)
    );

    // костыль
    if (radius < 0.001f)
    {
        radius = 2.0f;
    }

    // Calculate new position (orbit in XZ plane by default, you can modify this)
    float newX = orbitCenter.x + radius * cosf(currentOrbitAngle);
    float newZ = orbitCenter.z + radius * sinf(currentOrbitAngle);

    // Calculate delta translation
    DirectX::XMFLOAT3 delta(
        newX - position.x,
        0.0f, // Keep Y constant for planar orbit
        newZ - position.z
    );

    // Apply translation
    translate(delta);
}

void Planet::revolve(float deltaTime)
{
    if (!mesh) return;

    // Update rotation angle
    currentRotationAngle += rotationSpeed * deltaTime;

    // Keep angle in reasonable range
    if (currentRotationAngle > 2.0f * 3.14159265359f)
    {
        currentRotationAngle -= 2.0f * 3.14159265359f;
    }

    // Apply rotation to the mesh (rotate around Y axis by default)
    mesh->setRotation(DirectX::XMFLOAT3(0.0f, currentRotationAngle, 0.0f));
}

void Planet::update(float deltaTime)
{
    rotateAgainstParent(deltaTime);
    revolve(deltaTime);
    GameComponent::update(deltaTime);

    for (auto* child : childComponents)
    {
        if (child)
        {
            child->update(deltaTime);
        }
    }
}
void Planet::draw()
{
    if (mesh)
    {
        mesh->draw();
    }
}
void Planet::initialize()
{
    if (mesh)
    {
        mesh->initialize();
    }
}
void Planet::destroyResources()
{
    if (mesh)
    {
        mesh->destroyResources();
    }
}
void Planet::reload()
{
    if (mesh)
    {
        mesh->reload();
    }
}
void Planet::resolveCollision(GameComponent* other)
{
    if (mesh)
    {
        mesh->resolveCollision(other);
    }
}