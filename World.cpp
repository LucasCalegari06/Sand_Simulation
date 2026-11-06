/*
 * World.cpp
 *
 *  Created on: 23.07.2026
 *      Author: lucas
 */

#include "World.h"

World::World() : gravity(Globals::gravityX, Globals::gravityY), numberColumnsX(Globals::windowSizeX / (Globals::sandRadius * 2) + 1),
numberLinesY(Globals::windowSizeY / (Globals::sandRadius * 2) + 1), grid(numberLinesY, std::vector<std::vector<Sand*>>(numberColumnsX))
{
	// TODO Auto-generated constructor stub


}

World::~World()
{
	// TODO Auto-generated destructor stub
}

std::vector<Sand>& World::getSandList()
{
	return sandList;
}

void World::addNewSand(Sand newSand)
{
	sandList.push_back(newSand);
}

/*
 * Clears all particle pointers stored in the spatial grid from the previous frame.
 * Iterates through the sand particle list to reassign each grain to the grid.
 * Calculates grid coordinates (cellX, cellY) based on each particle's position.
 * Stores a pointer to each particle in its corresponding cell for fast collision checks.
 */
void World::updateGrid()
{
	//clears the grid
	for (int iY = 0; iY < numberLinesY; iY++)
	{
		for (int iX = 0; iX < numberColumnsX; iX++)
		{
			grid[iY][iX].clear();
		}
	}
	//updates the grid
	for (auto it = sandList.begin(); it != sandList.end(); it++)
	{
		int cellX = it->position.x / (Globals::sandRadius * 2); //we need 2 times radius (size of a cell in the grid)
		int cellY = it->position.y / (Globals::sandRadius * 2); //we need 2 times radius (size of a cell in the grid)
		if (cellX >= numberColumnsX) cellX = numberColumnsX - 1;
		if (cellX < 0) cellX = 0;
		if (cellY >= numberLinesY) cellY = numberLinesY - 1;
		if (cellY < 0) cellY = 0; //makes sure nothing gets out of the bounds of the grid

		grid[cellY][cellX].push_back(&(*it)); //saves the address of the grain in the grid
	}
}

/*
 * Iterates through the spatial grid to detect and handle interactions between sand particles.
 * Evaluates neighboring cells (current, right, down, right-down, and left-down) to ensure each pair is only checked once.
 * Resolves physical overlaps by directly updating particle positions to prevent interpenetration.
 * Calculates relative velocities and applies impulses based on the average coefficient of restitution to simulate collisions.
 */
void World::updateCollisions()
{
	for (int iY = 0; iY < numberLinesY; iY++)
	{
		for (int iX = 0; iX < numberColumnsX; iX++)
		{
			if (!grid[iY][iX].empty()) //if there is one or more grains in the cell
			{
				for (int actuallGrain = 0; actuallGrain < (int)grid[iY][iX].size(); actuallGrain++) //grid[iY][iX][actuallGrain] is the address of main grain
				{ //there might be more than only one grain in the cell we are looking in to, so need to look after all of them
					for (int jY = 0; jY < 2; jY++) //checks for its own cell, for right, right-down and down
					{
						for (int jX = 0; jX < 2; jX++) //checks for its own cell, for right, right-down and down
						{
							if (iX + jX < numberColumnsX && iY + jY < numberLinesY && !grid[iY + jY][iX + jX].empty()) //if is part of the grid and not empty
							{
								for (int foundGrain = 0; foundGrain < (int)grid[iY + jY][iX + jX].size(); foundGrain++) //checks all grains in the cell
								{
									if(jX != 0 || jY != 0 || actuallGrain < foundGrain) // < so that it wont check A->B and later B->A
									{ //if we are not looking to the main grain. Using !=0 to avoid checking addresses every time (too slow)
										if (!grid[iY][iX][actuallGrain]->isAwake && !grid[iY + jY][iX + jX][foundGrain]->isAwake) //if both sleeping
										{
											continue;
										}
										if (grid[iY][iX][actuallGrain]->checkCollision(*(grid[iY + jY][iX + jX][foundGrain]))) //if there is collision
										{ //the vector contains Sand*
											grid[iY][iX][actuallGrain]->isAwake = true; //makes sure it not sleeping anymore
											grid[iY + jY][iX + jX][foundGrain]->isAwake = true; //makes sure it not sleeping anymore
											float overlap = grid[iY][iX][actuallGrain]->getOverlap(*(grid[iY + jY][iX + jX][foundGrain]));
											Vector2D unitaryDirection = grid[iY][iX][actuallGrain]->getUnitVectorDistance(*(grid[iY + jY][iX + jX][foundGrain]));
											grid[iY][iX][actuallGrain]->position.x += -overlap / 2 * unitaryDirection.x; //update position
											grid[iY][iX][actuallGrain]->position.y += -overlap / 2 * unitaryDirection.y; //update position
											grid[iY + jY][iX + jX][foundGrain]->position.x += overlap / 2 * unitaryDirection.x; //update position
											grid[iY + jY][iX + jX][foundGrain]->position.y += overlap / 2 * unitaryDirection.y; //update position

											Vector2D relVel = grid[iY][iX][actuallGrain]->getRelativeVelocity(*(grid[iY + jY][iX + jX][foundGrain]));
											float normalVelocity = relVel.dotProduct(unitaryDirection);
											if (normalVelocity > 0) //if the grains are going against each other. Else do nothing
											{ //take a look at my notes. There is the algebra explained
												float e1 = grid[iY][iX][actuallGrain]->coefficientOfRestitution;
												float e2 = grid[iY + jY][iX + jX][foundGrain]->coefficientOfRestitution;
												float eAvg = (e1 + e2) / 2; //average of the coefficients from both bodies
												float m1 = grid[iY][iX][actuallGrain]->mass;
												float m2 = grid[iY + jY][iX + jX][foundGrain]->mass;
												float propFactor = 1 / (m1 + m2);
												float impulse = normalVelocity * (1 + eAvg);
												grid[iY][iX][actuallGrain]->velocity.x -= impulse * unitaryDirection.x * m2 * propFactor; //update velocity
												grid[iY][iX][actuallGrain]->velocity.y -= impulse * unitaryDirection.y * m2 * propFactor; //update velocity
												grid[iY + jY][iX + jX][foundGrain]->velocity.x += impulse * unitaryDirection.x * m1 * propFactor; //update velocity
												grid[iY + jY][iX + jX][foundGrain]->velocity.y += impulse * unitaryDirection.y * m1 * propFactor; //update velocity

												Vector2D tanVel(relVel.x - normalVelocity * unitaryDirection.x, relVel.y - normalVelocity * unitaryDirection.y);
												if (tanVel.getSquaredAbs() < Globals::sandStaticFricCoefficient * Globals::sandStaticFricCoefficient)
												{
													float mEff = 1 / ((1  / m1) + (1 / m2));
													float fricImpulsX = - tanVel.x * mEff;
													float fricImpulsY = - tanVel.y * mEff;
													grid[iY][iX][actuallGrain]->velocity.x += fricImpulsX / m1;
													grid[iY][iX][actuallGrain]->velocity.y += fricImpulsY / m1;
													grid[iY + jY][iX + jX][foundGrain]->velocity.x -= fricImpulsX / m2;
													grid[iY + jY][iX + jX][foundGrain]->velocity.y -= fricImpulsY / m2;
												}
												else
												{
													float dVFrictionX = - tanVel.x * Globals::sandFrictionCoefficient;
													float dVFrictionY = - tanVel.y * Globals::sandFrictionCoefficient;
													float propFactor = 1 / (m1 + m2);
													grid[iY][iX][actuallGrain]->velocity.x += dVFrictionX * m2 * propFactor;
													grid[iY][iX][actuallGrain]->velocity.y += dVFrictionY * m2 * propFactor;
													grid[iY + jY][iX + jX][foundGrain]->velocity.x -= dVFrictionX * m1 * propFactor;
													grid[iY + jY][iX + jX][foundGrain]->velocity.y -= dVFrictionY * m1 * propFactor;
												}
											}
										}
									}
								}
							}
						}
					}
					if (iX - 1 >= 0 && iY + 1 < numberLinesY && !grid[iY +1][iX - 1].empty()) //checks left-down (total with the last checks: 5 checks/cell)
					{
						for (int foundGrain = 0; foundGrain < (int)grid[iY + 1][iX - 1].size(); foundGrain++)
						{
							if (!grid[iY][iX][actuallGrain]->isAwake && !grid[iY + 1][iX - 1][foundGrain]->isAwake) //if both sleeping
							{
								continue;
							}
							if (grid[iY][iX][actuallGrain]->checkCollision(*(grid[iY + 1][iX - 1][foundGrain]))) //if there is collision
							{ //the vector contains Sand*
								grid[iY][iX][actuallGrain]->isAwake = true; //makes sure it not sleeping anymore
								grid[iY + 1][iX - 1][foundGrain]->isAwake = true; //makes sure it not sleeping anymore
								float overlap = grid[iY][iX][actuallGrain]->getOverlap(*(grid[iY + 1][iX - 1][foundGrain]));
								Vector2D unitaryDirection = grid[iY][iX][actuallGrain]->getUnitVectorDistance(*(grid[iY + 1][iX - 1][foundGrain]));
								grid[iY][iX][actuallGrain]->position.x += -overlap / 2 * unitaryDirection.x; //update position
								grid[iY][iX][actuallGrain]->position.y += -overlap / 2 * unitaryDirection.y; //update position
								grid[iY + 1][iX - 1][foundGrain]->position.x += overlap / 2 * unitaryDirection.x; //update position
								grid[iY + 1][iX - 1][foundGrain]->position.y += overlap / 2 * unitaryDirection.y; //update position

								Vector2D relVel = grid[iY][iX][actuallGrain]->getRelativeVelocity(*(grid[iY + 1][iX - 1][foundGrain]));
								float normalVelocity = relVel.dotProduct(unitaryDirection);
								if (normalVelocity > 0) //if the grains are going against each other. Else do nothing
								{ //take a look at my notes. There is the algebra explained
									float e1 = grid[iY][iX][actuallGrain]->coefficientOfRestitution;
									float e2 = grid[iY + 1][iX - 1][foundGrain]->coefficientOfRestitution;
									float eAvg = (e1 + e2) / 2; //average of the coefficients from both bodies
									float m1 = grid[iY][iX][actuallGrain]->mass;
									float m2 = grid[iY + 1][iX - 1][foundGrain]->mass;
									float propFactor = 1 / (m1 + m2);
									float impulse = normalVelocity * (1 + eAvg);
									grid[iY][iX][actuallGrain]->velocity.x -= impulse * unitaryDirection.x * m2 * propFactor; //update velocity
									grid[iY][iX][actuallGrain]->velocity.y -= impulse * unitaryDirection.y * m2 * propFactor; //update velocity
									grid[iY + 1][iX - 1][foundGrain]->velocity.x += impulse * unitaryDirection.x * m1 * propFactor; //update velocity
									grid[iY + 1][iX - 1][foundGrain]->velocity.y += impulse * unitaryDirection.y * m1 * propFactor; //update velocity

									Vector2D tanVel(relVel.x - normalVelocity * unitaryDirection.x, relVel.y - normalVelocity * unitaryDirection.y);
									if (tanVel.getSquaredAbs() < Globals::sandStaticFricCoefficient * Globals::sandStaticFricCoefficient)
									{//if static friction triggers. Take a look at my notes for the formulas
										float mEff = 1 / ((1  / m1) + (1 / m2));
										float fricImpulsX = - tanVel.x * mEff;
										float fricImpulsY = - tanVel.y * mEff;
										grid[iY][iX][actuallGrain]->velocity.x += fricImpulsX / m1;
										grid[iY][iX][actuallGrain]->velocity.y += fricImpulsY / m1;
										grid[iY + 1][iX - 1][foundGrain]->velocity.x -= fricImpulsX / m2;
										grid[iY + 1][iX - 1][foundGrain]->velocity.y -= fricImpulsY / m2;
									}
									else
									{//if dynamic friction triggers. Take a look at my notes for the formulas
										float dVFrictionX = - tanVel.x * Globals::sandFrictionCoefficient;
										float dVFrictionY = - tanVel.y * Globals::sandFrictionCoefficient;
										float propFactor = 1 / (m1 + m2);
										grid[iY][iX][actuallGrain]->velocity.x += dVFrictionX * m2 * propFactor;
										grid[iY][iX][actuallGrain]->velocity.y += dVFrictionY * m2 * propFactor;
										grid[iY + 1][iX - 1][foundGrain]->velocity.x -= dVFrictionX * m1 * propFactor;
										grid[iY + 1][iX - 1][foundGrain]->velocity.y -= dVFrictionY * m1 * propFactor;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void World::updateWorld(float dt)
{
	for (auto it = sandList.begin(); it != sandList.end(); it++)
	{
		if (it->isAwake == true)
		{
			it->velocity.y += gravity.y * dt; //increases the velocity for every sand. Gravity accelerates the sand
			it->updatePosition(dt); //with the new velocity we can now update the position of the particle
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//we will add as test coordinates for the floor and the walls, the code will work and we'll be able to see how the physics are working
		//that part will be later removed of the code, it is only there for we to make it work in a easier map, in order to see it working
		//we'll define as borders of a box: floor = 600, left wall = 0 and right wall = 800
		//position is the center of the particle, so we need to consider the radius, in order that nothing of the particle gets through the walls
		//if something passes the wall or the floor, it's position gets updated and it velocity in that direction is set to zero
		/*if (it->position.y + it->radius > Globals::windowSizeY)
		{
			it->position.y = Globals::windowSizeY - it->radius;
			it->velocity.y = 0;
		}
		if (it->position.y - it->radius < 0)
		{
			it->position.y = 0 + it->radius;
			it->velocity.y = 0;
		}
		if (it->position.x - it->radius < 0)
		{
			it->position.x = 0 + it->radius;
			it->velocity.x = 0;
		}
		if (it->position.x + it->radius > Globals::windowSizeX)
		{
			it->position.x = Globals::windowSizeX - it->radius;
			it->velocity.x = 0;
		}*/
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	this->updateGrid();

	//we have the updated grid. now we can check for collisions
	this->updateCollisions();

	for (auto it = sandList.begin(); it != sandList.end(); it++)
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//we will add as test coordinates for the floor and the walls, the code will work and we'll be able to see how the physics are working
		//that part will be later removed of the code, it is only there for we to make it work in a easier map, in order to see it working
		//we'll define as borders of a box: floor = 600, left wall = 0 and right wall = 800
		//position is the center of the particle, so we need to consider the radius, in order that nothing of the particle gets through the walls
		//if something passes the wall or the floor, it's position gets updated and it velocity in that direction is set to zero
		if (it->position.y + it->radius > Globals::windowSizeY)
		{
			it->position.y = Globals::windowSizeY - it->radius;
			it->velocity.y = 0;

			if (it->velocity.x * it->velocity.x < Globals::sandStaticFricCoefficient * Globals::sandStaticFricCoefficient) //it.velocity.x is here tanVel
			{//if static friction triggers. Take a look at my notes for the formulas
				it->velocity.x = 0; //relVel is in that case always = it->velocity.x
			}
			else
			{//if dynamic friction triggers. Take a look at my notes for the formulas
				it->velocity.x *= (1.0f - Globals::sandFrictionCoefficient);
			}
		}
		if (it->position.y - it->radius < 0)
		{
			it->position.y = 0 + it->radius;
			it->velocity.y = 0;

			if (it->velocity.x * it->velocity.x < Globals::sandStaticFricCoefficient * Globals::sandStaticFricCoefficient) //it.velocity.x is here tanVel
			{//if static friction triggers. Take a look at my notes for the formulas
				it->velocity.x = 0; //relVel is in that case always = it->velocity.x
			}
			else
			{//if dynamic friction triggers. Take a look at my notes for the formulas
				it->velocity.x *= (1.0f - Globals::sandFrictionCoefficient);
			}
		}
		if (it->position.x - it->radius < 0)
		{
			it->position.x = 0 + it->radius;
			it->velocity.x = 0;

			if (it->velocity.y * it->velocity.y < Globals::sandStaticFricCoefficient * Globals::sandStaticFricCoefficient) //it.velocity.x is here tanVel
			{//if static friction triggers. Take a look at my notes for the formulas
				it->velocity.y = 0; //relVel is in that case always = it->velocity.x
			}
			else
			{//if dynamic friction triggers. Take a look at my notes for the formulas
				it->velocity.y *= (1.0f - Globals::sandFrictionCoefficient);
			}
		}
		if (it->position.x + it->radius > Globals::windowSizeX)
		{
			it->position.x = Globals::windowSizeX - it->radius;
			it->velocity.x = 0;

			if (it->velocity.y * it->velocity.y < Globals::sandStaticFricCoefficient * Globals::sandStaticFricCoefficient) //it.velocity.x is here tanVel
			{//if static friction triggers. Take a look at my notes for the formulas
				it->velocity.y = 0; //relVel is in that case always = it->velocity.x
			}
			else
			{//if dynamic friction triggers. Take a look at my notes for the formulas
				it->velocity.y *= (1.0f - Globals::sandFrictionCoefficient);
			}
		}

		if (it->isAwake == true && it->getSquaredVelocity() < Globals::sleepVelocityThreshold * Globals::sleepVelocityThreshold)
		{
			if (it->sleepTimerFrames < Globals::sleepTimeThreshold)
			{
				it->sleepTimerFrames += (1.0f / Globals::updatesPerFrame); //since we're counting per frame and we use this more than once per frame
			}
			else
			{
				it->velocity.x = 0;
				it->velocity.y = 0;
				it->sleepTimerFrames = 0.0f;
				it->isAwake = false;
			}
		}
		else if (it->isAwake == true && it->sleepTimerFrames > 0.0f)
		{
			it->sleepTimerFrames = 0.0f;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
}
