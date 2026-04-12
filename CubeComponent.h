#pragma once
#include "MeshComponent.h"

class CubeComponent : public MeshComponent
{
public:
    CubeComponent(Game* g, const DirectX::XMFLOAT4& color, float size = 1.0f);
    virtual ~CubeComponent() = default;

    virtual void update() override;

    void setSize(float size);
    float getSize() const { return cubeSize; }

protected:
    virtual void generateMesh() override;

private:
    float cubeSize;
};