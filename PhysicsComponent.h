#pragma once
#include <directxmath.h>

class GameComponent;

class PhysicsComponent
{
public:
	PhysicsComponent();
	~PhysicsComponent();

	// Mass and kinematic properties
	void setMass(float mass);
	float getMass() const;

	bool isKinematic() const;
	void setKinematic(bool kinematic);

	// Velocity
	void setVelocity(DirectX::XMFLOAT3 velocity);
	DirectX::XMFLOAT3 getVelocity() const;

	// Forces and impulses
	void applyForce(DirectX::XMFLOAT3 force);
	void applyImpulse(DirectX::XMFLOAT3 impulse);

	// Gravity and drag
	void setGravity(DirectX::XMFLOAT3 gravity);
	DirectX::XMFLOAT3 getGravity() const;

	void setDrag(float drag);
	float getDrag() const;

	// Restitution (bounciness: 0 = no bounce, 1 = full bounce)
	void setRestitution(float restitution);
	float getRestitution() const;

	// Physics simulation
	void update(float deltaTime, GameComponent* component);

	// AABB Collision detection
	bool checkAABBCollision(
		const PhysicsComponent* other,
		const DirectX::XMFLOAT3& thisPosition,
		const DirectX::XMFLOAT3& thisScale,
		const DirectX::XMFLOAT3& otherPosition,
		const DirectX::XMFLOAT3& otherScale) const;

	// AABB Collision resolution - prevents overlap
	void resolveAABBCollision(
		PhysicsComponent* other,
		GameComponent* thisComponent,
		GameComponent* otherComponent);

	DirectX::XMFLOAT3 getAABBMin(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale) const;
	DirectX::XMFLOAT3 getAABBMax(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale) const;

private:
	float mass;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 acceleration;
	DirectX::XMFLOAT3 gravity;
	float drag;
	float restitution;
	bool kinematic;
};