#include "PongBall.h"
#include "Game.h"

PongBall::PongBall(Game* game, DirectX::XMFLOAT4 color)
	: RectangleComponent(game, color)
{
	setScale(DirectX::XMFLOAT3(BALL_SCALE, BALL_SCALE, 1.0f));
	getPhysics()->setKinematic(false);
	getPhysics()->setGravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	getPhysics()->setRestitution(2.0f); // Полная упругость для отскока
	getPhysics()->setDrag(0.0f); // Без сопротивления воздуха
	getPhysics()->setMass(0.1f); // Масса мяча 
}

PongBall::~PongBall()
{
}

void PongBall::update()
{
	RectangleComponent::update();
}

bool PongBall::isOutOfBounds() const
{
	if (game->players.size() < 2) {
		return true;
	}

	float ballY = getPosition().y;
	float player1Y = game->players[0]->getPosition().y;
	float player2Y = game->players[1]->getPosition().y;

	// Ball is out of bounds if below player 1 (first element) or above player 2 (second element)
	if (ballY < player1Y || ballY > player2Y) {
		return true;
	}

	return false;
}