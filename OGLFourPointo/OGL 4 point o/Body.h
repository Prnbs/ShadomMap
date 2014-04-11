#ifndef _BODY
#define _BODY
#include "Utils.h"
#include "CurrentState.h"
#include <iostream>
using namespace std;

class Body
{
public:
	GLuint
		ProjectionMatrixUniformLocation,
		ViewMatrixUniformLocation,
		ModelMatrixUniformLocation,
		MVMatrixUniformLocation,
		ViewVectorUniformLocation;

	GLuint BufferIds[3];
	GLuint ShaderIds[3];
	GLuint texId;
			
	Matrix ModelMatrix;
	CurrentState state;

	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual void Draw() = 0;
	virtual GLuint GetViewMatrixUniformLocation() = 0;
	virtual GLuint GetProjectionMatrixUniformLocation() = 0;
	virtual GLuint GetProgramId() = 0;
	virtual Matrix GetModelMatrix();
	virtual void SetModelMatrix(Matrix);
};

#endif