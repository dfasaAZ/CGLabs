#pragma once
#include "RectangleComponent.h"

class PongBall : public RectangleComponent
{
public:
	PongBall(Game* game, DirectX::XMFLOAT4 color);
	~PongBall();

	void update(float deltaTime) override;
	int isOutOfBounds() const;

private:
	static constexpr float BALL_SCALE = 0.05f;
};