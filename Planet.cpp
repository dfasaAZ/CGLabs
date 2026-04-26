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
{
    if (mesh)
    {
        orbitCenter = this->getPosition();
        addChild(mesh);
		mesh->setPosition(this->position.x, this->position.y, this->position.z);
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
        Planet* planetChild = dynamic_cast<Planet*>(child);
        if (planetChild)
        {
            DirectX::XMFLOAT3 childPos = planetChild->getPosition();
            float dx = childPos.x - position.x;
            float dz = childPos.z - position.z;
            planetChild->orbitRadius = sqrtf(dx * dx + dz * dz);
            planetChild->currentOrbitAngle = atan2f(dz, dx);
            planetChild->setOrbitCenter(this->position);
        }
    }
}

void Planet::rotateAgainstParent(float deltaTime)
{
	//if (isControlledByPlayer) return;
    currentOrbitAngle += orbitSpeed * deltaTime;

    while (currentOrbitAngle > 2.0f * DirectX::XM_PI)
        currentOrbitAngle -= 2.0f * DirectX::XM_PI;
    while (currentOrbitAngle < 0.0f)
        currentOrbitAngle += 2.0f * DirectX::XM_PI;

    float newX = orbitCenter.x + orbitRadius * cosf(currentOrbitAngle);
    float newZ = orbitCenter.z + orbitRadius * sinf(currentOrbitAngle);

    DirectX::XMFLOAT3 delta(
        newX - position.x,
        0.0f,
        newZ - position.z
    );

    translate(delta);

    for (auto* moon : childComponents)
    {
        Planet* planetChild = dynamic_cast<Planet*>(moon);
        if (planetChild)
        {
            planetChild->setOrbitCenter(this->position);
        }
    }
}

void Planet::revolve(float deltaTime)
{
    if (!mesh) return;

    currentRotationAngle += rotationSpeed * deltaTime;

    while (currentRotationAngle > 2.0f * DirectX::XM_PI)
        currentRotationAngle -= 2.0f * DirectX::XM_PI;
    while (currentRotationAngle < 0.0f)
        currentRotationAngle += 2.0f * DirectX::XM_PI;

    mesh->setRotation(DirectX::XMFLOAT3(0.0f, currentRotationAngle, 0.0f));
}

void Planet::update(float deltaTime)
{
    GameComponent::update(deltaTime);
    rotateAgainstParent(deltaTime);
    revolve(deltaTime);

    for (auto* child : childComponents)
    {
        Planet* moon = dynamic_cast<Planet*>(child);
        if (moon)
        {
            moon->setOrbitCenter(this->position);
            moon->update(deltaTime);
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
void Planet::setPosition(float x, float y, float z)
{
    std::vector<DirectX::XMFLOAT3> childRelativePositions;
    for (auto* child : childComponents)
    {
        DirectX::XMFLOAT3 childPos = child->getPosition();
        childRelativePositions.push_back({
            childPos.x - position.x,
            childPos.y - position.y,
            childPos.z - position.z
            });
    }

    GameComponent::setPosition(x, y, z);
    this->orbitCenter = this->position;


    int index = 0;
    for (auto* child : childComponents)
    {
        if (index < childRelativePositions.size())
        {
            child->setPosition(
                x + childRelativePositions[index].x,
                y + childRelativePositions[index].y,
                z + childRelativePositions[index].z
            );

            Planet* moon = dynamic_cast<Planet*>(child);
            if (moon)
            {
                moon->setOrbitCenter(this->position);
            }
        }
        index++;
    }
}
void Planet::setScale(float x, float y, float z)
{
    GameComponent::setScale(x, y, z);
    if (mesh)
    {
        mesh->setScale(x, y, z);
    }
}
void Planet::translate(DirectX::XMFLOAT3 delta)
{
    GameComponent::translate(delta);

    this->orbitCenter.x += delta.x;
    this->orbitCenter.y += delta.y;
    this->orbitCenter.z += delta.z;
   
}