#include "PongBall.h"
#include "Game.h"

PongBall::PongBall(Game* game, DirectX::XMFLOAT4 color)
	: RectangleComponent(game, color)
{
	setScale(DirectX::XMFLOAT3(BALL_SCALE, BALL_SCALE, 1.0f));
	getPhysics()->setKinematic(false);
	getPhysics()->setGravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	getPhysics()->setRestitution(2.0f); 
	getPhysics()->setDrag(0.0f); 
	getPhysics()->setMass(0.1f); 
}

PongBall::~PongBall()
{
}

void PongBall::update(float deltaTime)
{
	RectangleComponent::update(deltaTime);
	this->rotate(DirectX::XMFLOAT3(0.0f, 0.0f, 5.0f));
}
//Возвращает 1, если мяч в воротах первого, 2 - второго,0 - на поле
int PongBall::isOutOfBounds() const
{
	if (game->players.size() < 4) {
		return true;
	}

	float ballY = getPosition().y;
	// Первые два игрока (0 и 1) - нижняя команда, вторые два (2 и 3) - верхняя команда
	float player1Y = game->players[0]->getPosition().y; 
	float player2Y = game->players[2]->getPosition().y;  

	// Ball is out of bounds if below player 1 (first element) or above player 2 (second element)
	if (ballY < player1Y)
	{
		return 1;
	}
	else
		if (ballY > player2Y) {
			return 2;
		}

	return 0;
}