#pragma once
#include <directxmath.h>

struct VertexPositionColorNormal
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT3 normal;

    VertexPositionColorNormal()
        : position(0, 0, 0), color(1, 1, 1, 1), normal(0, 1, 0) {
    }

    VertexPositionColorNormal(const DirectX::XMFLOAT3& pos,
        const DirectX::XMFLOAT4& col,
        const DirectX::XMFLOAT3& norm)
        : position(pos), color(col), normal(norm) {
    }
};