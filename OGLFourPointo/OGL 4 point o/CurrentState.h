#ifndef _CURRSTATE
#define _CURRSTATE
#include "Utils.h"

class CurrentState
{
public:
	static Vector Eye,
				  LookAt;
	static Matrix 
				ProjectionMatrix,
				ViewMatrix;
	static clock_t 
				LastTime;

    Vector GetEye();
	Vector GetLookAt();
	Matrix GetProjectionMatrix();
	Matrix GetViewMatrix();
	clock_t GetLastTime();

	void SetEye(Vector);
	void SetLookAt(Vector);
	void SetProjectionMatrix(Matrix);
	void SetViewMatrix(Matrix);
	void SetLastTime(clock_t);
	void SetProgramId(GLuint);
};

#endif