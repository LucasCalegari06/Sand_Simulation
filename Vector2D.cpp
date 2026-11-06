/*
 * Vector2D.cpp
 *
 *  Created on: 23.07.2026
 *      Author: lucas
 */

#include "Vector2D.h"
#include <cmath>

Vector2D::Vector2D()
{
	// TODO Auto-generated constructor stub

}

Vector2D::Vector2D(float pX, float pY) : x(pX), y(pY)
{

}

Vector2D::~Vector2D()
{
	// TODO Auto-generated destructor stub
}

float Vector2D::getLengh() const
{
	return sqrt(this->x * this->x + this->y * this->y);
}

float Vector2D::dotProduct(const Vector2D& vec2) const
{
	return this->x * vec2.x + this->y * vec2.y;
}

float Vector2D::getSquaredAbs() const
{
	return x * x + y * y;
}
