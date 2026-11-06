/*
 * World.h
 *
 *  Created on: 23.07.2026
 *      Author: lucas
 */

#ifndef WORLD_H_
#define WORLD_H_
#include <iostream>
#include "Sand.h"
#include <vector>
#include "Globals.h"

class World
{
public:
	World();
	~World();
	std::vector<Sand>& getSandList();
	void addNewSand(Sand newSand);
	void updateWorld(float dt);


private:
	std::vector<Sand> sandList;
	Vector2D gravity;
	int numberColumnsX; //with +1 in constructor to avoid rounding errors
	int numberLinesY; //with +1 in constructor to avoid rounding errors
	std::vector<std::vector<std::vector<Sand*>>> grid; //2 physical dimensions (cell coordinates) and one vector to save all the grains inside a cell
	void updateGrid();
	void updateCollisions();
};

#endif /* WORLD_H_ */
