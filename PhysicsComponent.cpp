#include "PhysicsComponent.h"
#include "GameComponent.h"
#include <cmath>
#include <algorithm>

PhysicsComponent::PhysicsComponent()
	: mass(1.0f)
	, velocity(0.0f, 0.0f, 0.0f)
	, acceleration(0.0f, 0.0f, 0.0f)
	, gravity(0.0f, -9.81f, 0.0f)
	, drag(0.9f)
	, restitution(0.0f)
	, kinematic(1)
{
}

PhysicsComponent::~PhysicsComponent()
{
}

void PhysicsComponent::setMass(float mass) {
	this->mass = mass > 0.0f ? mass : 1.0f;
}

float PhysicsComponent::getMass() const {
	return mass;
}

bool PhysicsComponent::isKinematic() const {
	return kinematic;
}

void PhysicsComponent::setKinematic(bool kinematic) {
	this->kinematic = kinematic;
	if (kinematic) {
		velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		acceleration = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}

void PhysicsComponent::setVelocity(DirectX::XMFLOAT3 velocity) {
	this->velocity = velocity;
}

DirectX::XMFLOAT3 PhysicsComponent::getVelocity() const {
	return velocity;
}

void PhysicsComponent::applyForce(DirectX::XMFLOAT3 force) {
	if (kinematic) return;
	acceleration.x += force.x / mass;
	acceleration.y += force.y / mass;
	acceleration.z += force.z / mass;
}

void PhysicsComponent::applyImpulse(DirectX::XMFLOAT3 impulse) {
	if (kinematic) return;
	velocity.x += impulse.x / mass;
	velocity.y += impulse.y / mass;
	velocity.z += impulse.z / mass;
}

void PhysicsComponent::setGravity(DirectX::XMFLOAT3 gravity) {
	this->gravity = gravity;
}

DirectX::XMFLOAT3 PhysicsComponent::getGravity() const {
	return gravity;
}

void PhysicsComponent::setDrag(float drag) {
	this->drag = drag >= 0.0f ? drag : 0.0f;
}

float PhysicsComponent::getDrag() const {
	return drag;
}

void PhysicsComponent::setRestitution(float restitution) {
	this->restitution = restitution >= 0.0f ? restitution : 0.0f;
}

float PhysicsComponent::getRestitution() const {
	return restitution;
}

void PhysicsComponent::update(float deltaTime, GameComponent* component) {
	if (kinematic || !component) return;

	// Apply gravity
	applyForce(DirectX::XMFLOAT3(
		gravity.x * mass,
		gravity.y * mass,
		gravity.z * mass));

	// Update velocity from acceleration
	velocity.x += acceleration.x * deltaTime;
	velocity.y += acceleration.y * deltaTime;
	velocity.z += acceleration.z * deltaTime;

	// Apply drag (air resistance)
	float dragFactor = 1.0f - (drag * deltaTime);
	dragFactor = dragFactor > 0.0f ? dragFactor : 0.0f;
	velocity.x *= dragFactor;
	velocity.y *= dragFactor;
	velocity.z *= dragFactor;

	// Update position using translate function
	component->translate(DirectX::XMFLOAT3(
		velocity.x * deltaTime,
		velocity.y * deltaTime,
		velocity.z * deltaTime));

	// Reset acceleration for next frame
	acceleration = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

bool PhysicsComponent::checkAABBCollision(
	const PhysicsComponent* other,
	const DirectX::XMFLOAT3& thisPosition,
	const DirectX::XMFLOAT3& thisScale,
	const DirectX::XMFLOAT3& otherPosition,
	const DirectX::XMFLOAT3& otherScale) const {

	if (!other) return false;

	DirectX::XMFLOAT3 thisMin = getAABBMin(thisPosition, thisScale);
	DirectX::XMFLOAT3 thisMax = getAABBMax(thisPosition, thisScale);
	DirectX::XMFLOAT3 otherMin = other->getAABBMin(otherPosition, otherScale);
	DirectX::XMFLOAT3 otherMax = other->getAABBMax(otherPosition, otherScale);

	return (thisMin.x <= otherMax.x && thisMax.x >= otherMin.x) &&
		(thisMin.y <= otherMax.y && thisMax.y >= otherMin.y) &&
		(thisMin.z <= otherMax.z && thisMax.z >= otherMin.z);
}

void PhysicsComponent::resolveAABBCollision(
	PhysicsComponent* other,
	DirectX::XMFLOAT3& thisPosition,
	const DirectX::XMFLOAT3& thisScale,
	DirectX::XMFLOAT3& otherPosition,
	const DirectX::XMFLOAT3& otherScale) {

	if (!other) return;

	DirectX::XMFLOAT3 thisMin = getAABBMin(thisPosition, thisScale);
	DirectX::XMFLOAT3 thisMax = getAABBMax(thisPosition, thisScale);
	DirectX::XMFLOAT3 otherMin = other->getAABBMin(otherPosition, otherScale);
	DirectX::XMFLOAT3 otherMax = other->getAABBMax(otherPosition, otherScale);

	// Calculate overlap on each axis
	float overlapLeft = thisMax.x - otherMin.x;
	float overlapRight = otherMax.x - thisMin.x;
	float overlapTop = thisMax.y - otherMin.y;
	float overlapBottom = otherMax.y - thisMin.y;
	float overlapFront = thisMax.z - otherMin.z;
	float overlapBack = otherMax.z - thisMin.z;

	// Find minimum overlap axis
	float minOverlap = overlapLeft;
	int axis = 0; // 0: left, 1: right, 2: top, 3: bottom, 4: front, 5: back

	if (overlapRight < minOverlap) { minOverlap = overlapRight; axis = 1; }
	if (overlapTop < minOverlap) { minOverlap = overlapTop; axis = 2; }
	if (overlapBottom < minOverlap) { minOverlap = overlapBottom; axis = 3; }
	if (overlapFront < minOverlap) { minOverlap = overlapFront; axis = 4; }
	if (overlapBack < minOverlap) { minOverlap = overlapBack; axis = 5; }

	// Separate objects based on minimum overlap
	float separation = minOverlap + 0.001f; // Small epsilon to prevent re-collision
	bool thisKinematic = this->isKinematic();
	bool otherKinematic = other->isKinematic();

	// Mass ratio for weighted separation
	float thisMass = thisKinematic ? 99990.0f : this->mass;
	float otherMass = otherKinematic ? 99990.0f : other->mass;
	float totalMass = thisMass + otherMass;

	if (totalMass == 0.0f) return; // Both kinematic

	float thisRatio = otherMass / totalMass;
	float otherRatio = thisMass / totalMass;

	// Apply separation
	switch (axis) {
		case 0: // Left
			thisPosition.x -= separation * thisRatio;
			otherPosition.x += separation * otherRatio;
			break;
		case 1: // Right
			thisPosition.x += separation * thisRatio;
			otherPosition.x -= separation * otherRatio;
			break;
		case 2: // Top
			thisPosition.y -= separation * thisRatio;
			otherPosition.y += separation * otherRatio;
			break;
		case 3: // Bottom
			thisPosition.y += separation * thisRatio;
			otherPosition.y -= separation * otherRatio;
			break;
		case 4: // Front
			thisPosition.z -= separation * thisRatio;
			otherPosition.z += separation * otherRatio;
			break;
		case 5: // Back
			thisPosition.z += separation * thisRatio;
			otherPosition.z -= separation * otherRatio;
			break;
	}

	// Apply restitution (bouncing)
	float e = (restitution < other->restitution) ? restitution : other->restitution;
	DirectX::XMFLOAT3 relativeVelocity;
	relativeVelocity.x = velocity.x - other->velocity.x;
	relativeVelocity.y = velocity.y - other->velocity.y;
	relativeVelocity.z = velocity.z - other->velocity.z;

	// Get collision normal from axis
	DirectX::XMFLOAT3 normal(0.0f, 0.0f, 0.0f);
	switch (axis) {
		case 0: normal.x = -1.0f; break;
		case 1: normal.x = 1.0f; break;
		case 2: normal.y = -1.0f; break;
		case 3: normal.y = 1.0f; break;
		case 4: normal.z = -1.0f; break;
		case 5: normal.z = 1.0f; break;
	}

	// Calculate impulse scalar
	float velAlongNormal = relativeVelocity.x * normal.x +
		relativeVelocity.y * normal.y +
		relativeVelocity.z * normal.z;

	// Only resolve if objects are moving towards each other
	if (velAlongNormal < 0.0f) {
		float restitutionScalar = -(1.0f + e) * velAlongNormal / totalMass;

		if (!thisKinematic) {
			velocity.x += normal.x * restitutionScalar * otherMass;
			velocity.y += normal.y * restitutionScalar * otherMass;
			velocity.z += normal.z * restitutionScalar * otherMass;
		}

		if (!otherKinematic) {
			other->velocity.x -= normal.x * restitutionScalar * thisMass;
			other->velocity.y -= normal.y * restitutionScalar * thisMass;
			other->velocity.z -= normal.z * restitutionScalar * thisMass;
		}
	}
}

DirectX::XMFLOAT3 PhysicsComponent::getAABBMin(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale) const {
	return DirectX::XMFLOAT3(
		position.x - scale.x * 0.5f,
		position.y - scale.y * 0.5f,
		position.z - scale.z * 0.5f);
}

DirectX::XMFLOAT3 PhysicsComponent::getAABBMax(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale) const {
	return DirectX::XMFLOAT3(
		position.x + scale.x * 0.5f,
		position.y + scale.y * 0.5f,
		position.z + scale.z * 0.5f);
}