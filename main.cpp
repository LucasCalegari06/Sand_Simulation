#include "raylib.h"
#include "World.h"
#include "Sand.h"
#include "Globals.h"
#include <random>

int main()
{
    // Initializes window
    InitWindow(Globals::windowSizeX, Globals::windowSizeY, "Falling Sand Simulator");
    SetTargetFPS(Globals::targetFPS); // sets target FPS

    // starts world and adds test particle
    World world;
    //Sand particle(Globals::windowSizeX / 2, 100.0f);
    //Sand particle2(Globals::windowSizeX / 2, 50.0f);// Constructor (x, y)
    //world.addNewSand(particle);
    //world.addNewSand(particle2);
    int counterFrames = 0; //only to delay the start
    int counterGrains = 0;
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> randNumber(0.0f, 5.0f); //starts the random generator outside the loop

    // main loop of the graphic interface
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime(); //gets the real dt (between frames)

        float i;
        if (counterFrames >= 5 && counterGrains < 3000)
        {
        	i = randNumber(generator);
        	Sand particle(Globals::windowSizeX / 2 + i, 100.0f);
        	world.addNewSand(particle);
        	counterGrains++;
        	counterFrames = 0;
        }
        else
        {
        	counterFrames++;
        }

        for (int i = 0; i < Globals::updatesPerFrame; i++) // sets the number of updates per frame
        {
        	world.updateWorld(dt/(float) Globals::updatesPerFrame); //divided by num of updates/frame because dt is the interval between two frames
        }

        // drawing
        BeginDrawing();
		ClearBackground(BLACK); // black background

            for (const auto& sand : world.getSandList()) //uses the getter to draw all particles
            {
            	if (sand.isAwake == true)
            	{
            		DrawCircle(sand.position.x, sand.position.y, sand.radius, YELLOW);
            	}
            	else
            	{
            		DrawCircle(sand.position.x, sand.position.y, sand.radius, YELLOW); //ignore
            	}
            }
        EndDrawing();
    }

    // closes the window
    CloseWindow();
    return 0;
}
