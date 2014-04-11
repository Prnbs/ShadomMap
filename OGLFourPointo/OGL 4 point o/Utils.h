/*	Copyright (C) 2011 by Eddy Luten

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/glext.h>
#include <GL/freeglut.h>

static const double PI = 3.14159265358979323846;

typedef struct Vertex
{
	float Position[4];
	float Color[4];
} Vertex;

typedef struct Matrix
{
	float m[16];
} Matrix;

typedef struct Vector
{
	float v[3];
} Vector;

static const Matrix IDENTITY_MATRIX = { {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
} };

static const Vector ORIGIN = { { 0, 0 , 0 } };

//typedef struct point {
//  GLfloat x;
//  GLfloat y;
//  GLfloat z;
//  GLfloat w;
//} Point;


#define N 100


static const Vector UP = { { 0, 1.0f, 0} };

float Cotangent(float angle);
float DegreesToRadians(float degrees);
float RadiansToDegrees(float radians);
Matrix MultiplyMatrices(const Matrix* m1, const Matrix* m2);
void RotateAboutX(Matrix* m, float angle);
void RotateAboutY(Matrix* m, float angle);
void RotateAboutZ(Matrix* m, float angle);
void ScaleMatrix(Matrix* m, float x, float y, float z);
void TranslateMatrix(Matrix* m, float x, float y, float z);
void CreateVector(Vector a, Vector b, Vector* res);
void Add(Vector a, Vector b, Vector* res);
float Dot(Vector a, Vector b);
void Cross(Vector a, Vector b, Vector* res);
void Normalize(Vector a);
void CreateUp(Vector a, Vector b, Vector* res);
void CreateColumnMajorMatrix(Vector row1, Vector row2, Vector row3, Matrix* out);
void CreateViewMatrix(Matrix* view, Vector eye, Vector lookAt);
Matrix InverseMatrix(const Matrix* m);
Matrix TransposeMatrix(const Matrix* m);
void PrintVertex(Vertex);

Matrix CreateProjectionMatrix(
	float fovy,
	float aspect_ratio,
	float near_plane,
	float far_plane
);

void ExitOnGLError(const char* error_message);
GLuint LoadShader(const char* filename, GLenum shader_type);

#endif