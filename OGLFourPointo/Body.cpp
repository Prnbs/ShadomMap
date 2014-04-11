#include "Body.h"

Matrix Body::GetModelMatrix()
{
	return ModelMatrix;
}

void Body::SetModelMatrix(Matrix data)
{
	memcpy(ModelMatrix.m, data.m, sizeof(data.m));
}