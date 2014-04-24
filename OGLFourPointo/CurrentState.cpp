#include "CurrentState.h"
#include "Utils.h"


Vector CurrentState::Eye = ORIGIN;
Vector CurrentState::LookAt = ORIGIN;
Matrix CurrentState::ProjectionMatrix = IDENTITY_MATRIX;
Matrix CurrentState::ShadowBiasMatrix = IDENTITY_MATRIX;
Matrix CurrentState::ViewMatrix = IDENTITY_MATRIX;
Matrix CurrentState::ActualViewMatrix = IDENTITY_MATRIX;
Matrix CurrentState::OrthoProjectionMatrix = IDENTITY_MATRIX;
Matrix CurrentState::ShadowViewMatrix = IDENTITY_MATRIX;
clock_t CurrentState::LastTime = 0;
Vector CurrentState::LightDir = ORIGIN;
bool CurrentState::shadowMat = false;
GLuint CurrentState::shadow_Fbuffer = 0;

GLuint CurrentState::shadow_tex = 0;
GLuint CurrentState::debug_shadow_tex = 0;

Vector CurrentState::GetEye()
{
	return Eye;
}

Vector CurrentState::GetLookAt()
{
	return LookAt;
}

clock_t CurrentState::GetLastTime()
{
	return LastTime;
}

Matrix CurrentState::GetProjectionMatrix()
{
	return ProjectionMatrix;
}

Matrix CurrentState::GetOrthoProjectionMatrix()
{
	return OrthoProjectionMatrix;
}

Matrix CurrentState::GetViewMatrix()
{
	return ViewMatrix;
}

Matrix CurrentState::GetActualViewMatrix()
{
	return ActualViewMatrix;
}

Matrix CurrentState::GetShadowViewMatrix()
{
	return ShadowViewMatrix;
}

Vector CurrentState::GetLightDirection()
{
	return LightDir;

}

void CurrentState::SetEye(Vector data)
{
	memcpy(Eye.v, data.v, sizeof(data.v));
}

void CurrentState::SetLookAt(Vector data)
{
	memcpy(LookAt.v, data.v, sizeof(data.v));
}

void CurrentState::SetProjectionMatrix(Matrix data)
{
	memcpy(ProjectionMatrix.m, data.m, sizeof(data.m));
}

void CurrentState::SetOrthoProjectionMatrix(Matrix data)
{
	memcpy(OrthoProjectionMatrix.m, data.m, sizeof(data.m));
}

void CurrentState::SetViewMatrix(Matrix data)
{
	memcpy(ViewMatrix.m, data.m, sizeof(data.m));
}

void CurrentState::SetActualViewMatrix(Matrix data)
{
	memcpy(ActualViewMatrix.m, data.m, sizeof(data.m));
}

void CurrentState::SetShadowViewMatrix(Matrix data)
{
	memcpy(ShadowViewMatrix.m, data.m, sizeof(data.m));
}

void CurrentState::SetLastTime(clock_t data)
{
	LastTime = data;
}

void CurrentState::SetLightDirection(Vector data)
{
	memcpy(LightDir.v, data.v, sizeof(data.v));
}

Matrix CurrentState::GetShadowBiasMatrix(Matrix ModelMatrix){
	if(shadowMat)
		return ShadowBiasMatrix;
	else{
		shadowMat = true;
		Matrix MV = MultiplyMatrices(&(GetShadowViewMatrix()), &ModelMatrix);
		Matrix MVP =  MultiplyMatrices(&(GetOrthoProjectionMatrix()), &MV);
		Matrix bias = IDENTITY_MATRIX;
		bias.m[0] = 0.5; bias.m[5] = 0.5; bias.m[10] = 0.5;
		bias.m[12] = 0.5; bias.m[13] = 0.5; bias.m[14] = 0.5;
		ShadowBiasMatrix = MultiplyMatrices(&ShadowBiasMatrix, &MVP);
		return ShadowBiasMatrix;
	}
}


