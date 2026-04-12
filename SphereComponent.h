#pragma once
#include "MeshComponent.h"

class SphereComponent : public MeshComponent
{
public:
    SphereComponent(Game* g, const DirectX::XMFLOAT4& color,
        float radius = 0.5f,
        uint32_t slices = 16,
        uint32_t stacks = 16);
    virtual ~SphereComponent() = default;

    void setRadius(float radius);
    float getRadius() const { return sphereRadius; }

    void setDetail(uint32_t slices, uint32_t stacks);

protected:
    virtual void generateMesh() override;

private:
    float sphereRadius;
    uint32_t sliceCount;
    uint32_t stackCount;
};