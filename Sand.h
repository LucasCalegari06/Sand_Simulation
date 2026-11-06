/*
 * Sand.h
 *
 *  Created on: 23.07.2026
 *      Author: lucas
 */

#ifndef SAND_H_
#define SAND_H_

#include <iostream>
#include "Vector2D.h"
#include "Globals.h"

class Sand
{
public:
	Sand();
	Sand(float pX, float pY);
	virtual ~Sand();
	void updatePosition(float dt);
	Vector2D position;
	Vector2D velocity;
	float mass;
	float radius; //sand is a circle
	float coefficientOfRestitution;
	bool isAwake; //turns the grain off for the gravity
	float sleepTimerFrames; //timer to decide if the grain can be turned off after not interacting with nothing
	bool checkCollision(const Sand& otherGrain) const;
	float getDistance(const Sand& otherGrain) const;
	Vector2D getUnitVectorDistance(const Sand& otherGrain) const;
	float getOverlap(const Sand& otherGrain) const;
	Vector2D getRelativeVelocity(const Sand& otherGrain) const;
	float getSquaredVelocity() const;
};

#endif /* SAND_H_ */
