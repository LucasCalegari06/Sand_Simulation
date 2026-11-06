#ifndef GLOBAlS_H
#define GLOBAlS_H


namespace Globals
{
	inline int targetFPS = 60;
    inline float sandMass = 1.0f;
    inline float sandRadius = 2.0f;
    inline float sandCoefficientOfRestitution = 0.05f;
    inline float sandFrictionCoefficient = 0.4f;
    inline float sandStaticFricCoefficient = 13.0f;
    inline int windowSizeX = 800;
	inline int windowSizeY = 600;
	inline float gravityX = 0.0f;
	inline float gravityY = 800.0f;
	inline int updatesPerFrame = 4;
	inline float sleepTimeThreshold = 7.0f; //in frames
	inline float sleepVelocityThreshold = 0.1f; //under this velocity it can sleep
}

#endif
