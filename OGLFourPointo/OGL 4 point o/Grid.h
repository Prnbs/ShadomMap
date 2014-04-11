#ifndef _GRID
#define _GRID

#include "Body.h"
#include "CurrentState.h"
#include "Utils.h"
#include <vector>
#include <iterator>

#define SEGMENT 64
#define NUM_GAUSS_TEXTURES 4

using namespace std;



class Grid : public Body
{
public:
	 GLushort indices[SEGMENT * SEGMENT * 6];

	 void Create();
	 void Destroy();
	 void Draw();

     GLuint g_gaussTextures[NUM_GAUSS_TEXTURES];
	 GLuint g_gaussSampler;

	 GLuint GetViewMatrixUniformLocation();
	 GLuint GetProjectionMatrixUniformLocation();
	 GLuint GetProgramId();
	 void BuildGaussianData(vector<GLubyte>&, int);
	 GLuint CreateGaussianTexture(int);
	 void CreateGaussianTextures();
	 int CalcCosAngResolution(int);

	 GLuint 
		 DirectionLightUniformLocation,
		 DirectionLightColourUniformLocation;
	 Vector DirectLight,
			ViewVect;
	 Vertex DirectLightColour;
	 GLuint VertNormBuff;
};

#endif