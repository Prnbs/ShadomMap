#ifndef _BODY
#define _BODY
#include "Utils.h"
#include "CurrentState.h"
#include <iostream>
#include <IL/il.h>
using namespace std;

class Body
{
public:
	GLuint
		ProjectionMatrixUniformLocation,
		OrthoProjectionMatrixUniformLocation,
		ViewMatrixUniformLocation,
		ModelMatrixUniformLocation,
		MVMatrixUniformLocation,
		ShadowBiasMatrixUniformLocation,
		ShadowMapUniformSampler,
		ShadowModelMatrixUniformLocation,
		ViewVectorUniformLocation;

	GLuint BufferIds[3];
	GLuint VertNormBuff,
			TexBuffer;

	GLuint image;

	bool success;

	
	GLuint g_gaussSampler, d_shadowSampler;
	GLuint gaussianTextureUnif;
	int g_gaussTexUnit;


	
	Matrix ModelMatrix;
	CurrentState state;

	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual void Draw(GLboolean) = 0;
	virtual void DrawFromLightPOV() = 0;
	virtual GLuint GetViewMatrixUniformLocation() = 0;
	virtual GLuint GetProjectionMatrixUniformLocation() = 0;
	virtual GLuint GetProgramId() = 0;
	virtual Matrix GetModelMatrix();
	virtual void SetModelMatrix(Matrix);
};

#endif