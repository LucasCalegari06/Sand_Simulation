/*
 * Sand.cpp
 *
 *  Created on: 23.07.2026
 *      Author: lucas
 */

#include "Sand.h"
#include <cmath>
#include <cstdlib> //for random direction in getUnitVectorDistance

Sand::Sand()
{
	// TODO Auto-generated constructor stub

}

Sand::Sand(float pX, float pY) : position(pX, pY), velocity(0, 0), mass(Globals::sandMass), radius(Globals::sandRadius),
		coefficientOfRestitution(Globals::sandCoefficientOfRestitution), isAwake(true), sleepTimerFrames(Globals::sleepTimeThreshold)
{

}

Sand::~Sand()
{
	// TODO Auto-generated destructor stub
}

void Sand::updatePosition(float dt)
{
	position.x = position.x + velocity.x * dt;
	position.y = position.y + velocity.y * dt;
}

bool Sand::checkCollision(const Sand& otherGrain) const
{
	float dX = this->position.x - otherGrain.position.x;
	float dY = this->position.y - otherGrain.position.y;
	float squaredDistance = dX * dX + dY * dY; //r^2 = dX^2 + dY^2 (we don't use pow because it is slow)
	float sumRadii = this->radius + otherGrain.radius; //radius + radius
	if (squaredDistance <= sumRadii * sumRadii) //r^2 <= (radius + radius)^2. We use ^2 to avoid using sqrt (slow).
	{
		return true;
	}
	return false;
}
float Sand::getDistance(const Sand& otherGrain) const
{
	float dX = this->position.x - otherGrain.position.x;
	float dY = this->position.y - otherGrain.position.y;
	return std::sqrt(dX * dX + dY * dY);
}
Vector2D Sand::getUnitVectorDistance(const Sand& otherGrain) const //d = otherGrain - this (unit vector to get the direction between to grains)
{
	Vector2D distanceVec(otherGrain.position.x - this->position.x, otherGrain.position.y - this->position.y);
	float distance = distanceVec.getLengh();
	if (distance < 0.01f) //if the grains are practically on the same spot
	{
		float randomDirection = ((float) rand()) / RAND_MAX; //random number between 0 and 1
		randomDirection = randomDirection * 2.0f * 3.14159f; //converting it to rad
		distanceVec.x = cos(randomDirection);
		distanceVec.y = sin(randomDirection); //works because of sin^2 + cos^2 = 1, exactly what we want
	}
	else
	{
		distanceVec.x = distanceVec.x / distance;
		distanceVec.y = distanceVec.y / distance;
	}
	return distanceVec;
}

float Sand::getOverlap(const Sand& otherGrain) const //how deep a grain is inside the other
{
	float dX = this->position.x - otherGrain.position.x;
	float dY = this->position.y - otherGrain.position.y;
	float distance = std::sqrt(dX * dX + dY * dY);
	return this->radius + otherGrain.radius - distance;
}

Vector2D Sand::getRelativeVelocity(const Sand& otherGrain) const //V1 (this) - V2 (otherGrain)
{
	Vector2D relVel(this->velocity.x - otherGrain.velocity.x, this->velocity.y - otherGrain.velocity.y);
	return relVel;
}

float Sand::getSquaredVelocity() const
{
	return velocity.x * velocity.x + velocity.y * velocity.y;
}
