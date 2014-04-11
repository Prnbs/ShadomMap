#ifndef _MATRIX
#define _MATRIX

class Matrix
{

typedef struct Matrix
{
	float m[16];
} Matrix;

static const Matrix IDENTITY_MATRIX = { {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
} };

Matrix MultiplyMatrices(const Matrix* m1, const Matrix* m2);

};

#endif