#ifndef _CURRSTATE
#define _CURRSTATE
#include "Utils.h"

class CurrentState
{
private:
    static Matrix ViewMatrix;
	static Matrix ActualViewMatrix;

public:
	static Vector Eye,
				  LookAt;
	static Matrix 
				ProjectionMatrix,
				OrthoProjectionMatrix,
				ShadowViewMatrix,
				ShadowBiasMatrix;
				
	static clock_t 
				LastTime;
	static Vector LightDir;

	static bool shadowMat;
	GLuint static shadow_Fbuffer, shadow_tex, debug_shadow_tex;

    Vector GetEye();
	Vector GetLookAt();
	Matrix GetProjectionMatrix();
	Matrix GetOrthoProjectionMatrix();
	Matrix GetViewMatrix();
	Matrix GetActualViewMatrix();
	Matrix GetShadowViewMatrix();
	Matrix GetShadowBiasMatrix(Matrix);
	clock_t GetLastTime();
	Vector GetLightDirection();

	GLuint ShaderShadowIds[3];
	

	void SetEye(Vector);
	void SetLookAt(Vector);
	void SetProjectionMatrix(Matrix);
	void SetOrthoProjectionMatrix(Matrix);
	void SetViewMatrix(Matrix);
	void SetActualViewMatrix(Matrix);
	void SetShadowViewMatrix(Matrix);
	void SetLastTime(clock_t);
	void SetProgramId(GLuint);
	void SetLightDirection(Vector);
};

#endif