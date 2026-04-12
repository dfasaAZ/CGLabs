#include "SphereComponent.h"
#include <cmath>

SphereComponent::SphereComponent(Game* g, const DirectX::XMFLOAT4& color,
    float radius, uint32_t slices, uint32_t stacks)
    : MeshComponent(g, color)
    , sphereRadius(radius)
    , sliceCount((((3u) > (slices)) ? (3u) : (slices)))
    , stackCount((((2u) > (stacks)) ? (2u) : (stacks)))
{
    generateMesh();
}

void SphereComponent::generateMesh()
{
    meshVertices.clear();
    meshIndices.clear();

    const float pi = 3.14159265359f;

    // Generate vertices
    for (uint32_t i = 0; i <= stackCount; ++i)
    {
        float phi = pi * float(i) / float(stackCount);
        float y = -cosf(phi);
        float ringRadius = sinf(phi);

        for (uint32_t j = 0; j <= sliceCount; ++j)
        {
            float theta = 2.0f * pi * float(j) / float(sliceCount);
            float x = ringRadius * cosf(theta);
            float z = ringRadius * sinf(theta);

            DirectX::XMFLOAT3 position(x * sphereRadius, y * sphereRadius, z * sphereRadius);
            DirectX::XMFLOAT3 normal(x, y, z); // Already normalized for unit sphere

            meshVertices.push_back(VertexPositionColorNormal(position, meshColor, normal));
        }
    }

    // Generate indices
    for (uint32_t i = 0; i < stackCount; ++i)
    {
        uint32_t currentRow = i * (sliceCount + 1);
        uint32_t nextRow = (i + 1) * (sliceCount + 1);

        for (uint32_t j = 0; j < sliceCount; ++j)
        {
            // First triangle
            meshIndices.push_back(currentRow + j);
            meshIndices.push_back(nextRow + j);
            meshIndices.push_back(nextRow + j + 1);

            // Second triangle
            meshIndices.push_back(currentRow + j);
            meshIndices.push_back(nextRow + j + 1);
            meshIndices.push_back(currentRow + j + 1);
        }
    }

    createBuffers(meshVertices, meshIndices);
}

void SphereComponent::setRadius(float radius)
{
    sphereRadius = radius;
    generateMesh();
}

void SphereComponent::setDetail(uint32_t slices, uint32_t stacks)
{
    sliceCount = (((3u) > (slices)) ? (3u) : (slices));
    stackCount = (((2u) > (stacks)) ? (2u) : (stacks));
    generateMesh();
}
void SphereComponent::update()
{
    MeshComponent::update();
 
}