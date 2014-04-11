#ifndef _CUBE
#define _CUBE

#include "Body.h"
#include "CurrentState.h"
#include "Utils.h"
#include <string>
#include <vector>

class Cube : public Body
{
public:
	GLuint 
		 DirectionLightUniformLocation,
		 DirectionLightColourUniformLocation,
		 ShadowProjectionMatrixUniformLocation,
		 ShadowViewMatrixUniformLocation,
		 ShadowModelMatrixUniformLocation;
	Vector DirectLight,
			ViewVect;
	vector<GLuint> ShaderCubeIds;
	GLuint bumpTex, teaTex;
	GLuint bumpSampler, teaSampler;
	GLuint WorldCameraPosUniformLoc;
	GLuint CubeMapTexUniLoc;
	GLuint bumpTextureUnif;
	GLuint ShadowBufferIds[3];
	   
	ILuint texid;
	ILboolean success; 
	int finished;
	float Xmin;
	float Xmax;
	float Ymin;
	float Ymax;
	float Zmin;
	float Zmax;
	float Xc, Zc;

	unsigned int height, width, format;

	 Vertex DirectLightColour;
	 GLuint INDICES[1024 * 3];
	 int vert;
	 float CubeRotation;
	 void Create();
	 void Destroy();
	 void Draw();
	 void DrawFromLightPOV();
	 void RenderFunction();
	 GLuint GetViewMatrixUniformLocation();
	 GLuint GetProjectionMatrixUniformLocation();
	 GLuint GetProgramId();
	 void FindMinAndMax(float, float, float);
	 void SetTexParams(string , GLuint &, GLuint &,GLuint );
		 
};

#endif