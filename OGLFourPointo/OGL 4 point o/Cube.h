#ifndef _CUBE
#define _CUBE

#include "Body.h"
#include "CurrentState.h"
#include "Utils.h"

class Cube : public Body
{
public:
	 GLuint 
		 DirectionLightUniformLocation,
		 DirectionLightColourUniformLocation;
	 Vector DirectLight,
			ViewVect;

	 Vertex DirectLightColour;
	 GLuint VertNormBuff;
	 GLuint INDICES[1024 * 3];
	 int vert;
	 float CubeRotation;
	 void Create();
	 void Destroy();
	 void Draw();
	 GLuint GetViewMatrixUniformLocation();
	 GLuint GetProjectionMatrixUniformLocation();
	 GLuint GetProgramId();
};

#endif