/*
 * Vector2D.h
 *
 *  Created on: 23.07.2026
 *      Author: lucas
 */

#ifndef VECTOR2D_H_
#define VECTOR2D_H_

#include "Globals.h"

class Vector2D {
public:
	Vector2D();
	Vector2D(float pX, float pY);
	~Vector2D();
	float x;
	float y;
	float getLengh() const;
	float dotProduct(const Vector2D& vec2) const;
	float getSquaredAbs() const;
};

#endif /* VECTOR2D_H_ */
