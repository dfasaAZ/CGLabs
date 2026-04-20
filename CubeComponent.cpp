#include "CubeComponent.h"

CubeComponent::CubeComponent(Game* g, const DirectX::XMFLOAT4& color, float size)
    : MeshComponent(g, color)
    , cubeSize(size)
{
    generateMesh();
}

void CubeComponent::generateMesh()
{
    meshVertices.clear();
    meshIndices.clear();

    float half = cubeSize * 0.5f;

    DirectX::XMFLOAT3 positions[] = {
        // Перед
        {-half, -half, -half}, { half, -half, -half}, { half,  half, -half}, {-half,  half, -half},
        // Зад
        { half, -half,  half}, {-half, -half,  half}, {-half,  half,  half}, { half,  half,  half},
        // Левый
        {-half, -half,  half}, {-half, -half, -half}, {-half,  half, -half}, {-half,  half,  half},
        // Правый
        { half, -half, -half}, { half, -half,  half}, { half,  half,  half}, { half,  half, -half},
        // Верзняя
        {-half,  half, -half}, { half,  half, -half}, { half,  half,  half}, {-half,  half,  half},
        // Нижняя
        {-half, -half,  half}, { half, -half,  half}, { half, -half, -half}, {-half, -half, -half}
    };

    DirectX::XMFLOAT3 normals[] = {
        { 0, 0,-1}, { 0, 0,-1}, { 0, 0,-1}, { 0, 0,-1}, 
        { 0, 0, 1}, { 0, 0, 1}, { 0, 0, 1}, { 0, 0, 1}, 
        {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, 
        { 1, 0, 0}, { 1, 0, 0}, { 1, 0, 0}, { 1, 0, 0}, 
        { 0, 1, 0}, { 0, 1, 0}, { 0, 1, 0}, { 0, 1, 0}, 
        { 0,-1, 0}, { 0,-1, 0}, { 0,-1, 0}, { 0,-1, 0}  
    };

    // Create vertices
    for (int i = 0; i < 24; ++i)
    {
        meshVertices.push_back(VertexPositionColorNormal(positions[i], meshColor, normals[i]));
    }

    // индексы для точек
    uint32_t faceIndices[] = {
        0,1,2, 2,3,0,   // Front
        4,5,6, 6,7,4,   // Back
        8,9,10, 10,11,8, // Left
        12,13,14, 14,15,12, // Right
        16,17,18, 18,19,16, // Top
        20,21,22, 22,23,20  // Bottom
    };

    for (int i = 0; i < 36; ++i)
    {
        meshIndices.push_back(faceIndices[i]);
    }

    createBuffers(meshVertices, meshIndices);
}

void CubeComponent::setSize(float size)
{
    cubeSize = size;
    generateMesh();
}
void CubeComponent::update(float deltaTime)
{
    MeshComponent::update(deltaTime);

    this->rotate(DirectX::XMFLOAT3(0.01f, 0.0f, 0.0f));
}