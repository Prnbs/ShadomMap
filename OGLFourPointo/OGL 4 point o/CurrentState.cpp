#include "CurrentState.h"
#include "Utils.h"


Vector CurrentState::Eye = ORIGIN;
Vector CurrentState::LookAt = ORIGIN;
Matrix CurrentState::ProjectionMatrix = IDENTITY_MATRIX;
Matrix CurrentState::ViewMatrix = IDENTITY_MATRIX;
clock_t CurrentState::LastTime = 0;

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

Matrix CurrentState::GetViewMatrix()
{
	return ViewMatrix;
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

void CurrentState::SetViewMatrix(Matrix data)
{
	memcpy(ViewMatrix.m, data.m, sizeof(data.m));
}



void CurrentState::SetLastTime(clock_t data)
{
	LastTime = data;
}

