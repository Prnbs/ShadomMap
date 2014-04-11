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

#include "Utils.h"
#include <iostream>

float Cotangent(float angle)
{
	return (float)(1.0 / tan(angle));
}

float DegreesToRadians(float degrees)
{
	return degrees * (float)(PI / 180);
}

float RadiansToDegrees(float radians)
{
	return radians * (float)(180 / PI);
}

Matrix MultiplyMatrices(const Matrix* m1, const Matrix* m2)
{
	unsigned int row, column, row_offset;
	Matrix out = IDENTITY_MATRIX;

	for(row = 0, row_offset = row * 4; row < 4; row++, row_offset = row * 4)
		for(column = 0; column < 4; column++)
			out.m[row_offset + column] = (m1->m[row_offset + 0] * m2->m[column + 0]) +
										 (m1->m[row_offset + 1] * m2->m[column + 4]) + 
										 (m1->m[row_offset + 2] * m2->m[column + 8]) +
										 (m1->m[row_offset + 3] * m2->m[column + 12]);
	return out;

}

void RotateAboutX(Matrix* m, float angle)
{
	Matrix rot = IDENTITY_MATRIX;

	float sine = (float) sin(angle);
	float cosine = (float) cos(angle);

	rot.m[5]  = cosine;
	rot.m[6]  = -sine;
	rot.m[9]  = sine;
	rot.m[10] = cosine;

	memcpy(m->m, MultiplyMatrices(m, &rot).m, sizeof(m->m));
}

void TranslatePointOnVector(Vertex v, Vector vec, Vertex* result, float distance)
{
	result->Position[0] = v.Position[0] - (distance * vec.v[0]);
	result->Position[1] = v.Position[1] - (distance * vec.v[1]);
	result->Position[2] = v.Position[2] - (distance * vec.v[2]);
}

void RotateAboutY(Matrix* m, float angle)
{
	Matrix rot = IDENTITY_MATRIX;

	float sine = (float) sin(angle);
	float cosine = (float) cos(angle);

	rot.m[0] = cosine;
	rot.m[2] = sine;
	rot.m[8] = -sine;
	rot.m[10] = cosine;

	memcpy(m->m, MultiplyMatrices(m, &rot).m, sizeof(m->m));
}

void RotateAboutZ(Matrix* m, float angle)
{
	Matrix rot = IDENTITY_MATRIX;

	float sine = (float) sin(angle);
	float cosine = (float) cos(angle);

	rot.m[0] = cosine;
	rot.m[1] = -sine;
	rot.m[4] = sine;
	rot.m[5] = cosine;

	memcpy(m->m, MultiplyMatrices(m, &rot).m, sizeof(m->m));
}

void ScaleMatrix(Matrix* m, float x, float y, float z)
{
	Matrix scale = IDENTITY_MATRIX;

	scale.m[0] = x;
	scale.m[5] = y;
	scale.m[10] = z;

	memcpy(m->m, MultiplyMatrices(m, &scale).m, sizeof(m->m));
}

void ScaleVector(Vector *vect, float scale, Vector *result){
	result->v[0] = vect->v[0]*scale;
	result->v[1] = vect->v[1]*scale;
	result->v[2] = vect->v[2]*scale;
	
}

void TranslateMatrix(Matrix* m, float x, float y, float z)
{
	Matrix translate = IDENTITY_MATRIX;
	
	translate.m[12] = x;
	translate.m[13] = y;
	translate.m[14] = z;

	memcpy(m->m, MultiplyMatrices(m, &translate).m, sizeof(m->m));
}

Matrix CreateProjectionMatrix(
	float fovy,
	float aspect_ratio,
	float near_plane,
	float far_plane
)
{
	Matrix out = { { 0 } };

	const float
		y_scale = Cotangent(DegreesToRadians(fovy / 2)),
		x_scale = y_scale / aspect_ratio,
		frustum_length = far_plane - near_plane;

	out.m[0] = x_scale;
	out.m[5] = y_scale;
	out.m[10] = -((far_plane + near_plane) / frustum_length);
	out.m[11] = -1;
	out.m[14] = -((2 * near_plane * far_plane) / frustum_length);
	
	return out;
}

Matrix CreateOrthoProjectionMatrix(
	float r, float l,
	float t, float b,
	float n, float f
)
{
	Matrix out = { { 0 } };

	out.m[0] = 2/(r-l);
	out.m[5] = 2/(t-b);
	out.m[10] = -1/(f-n);
	out.m[12] = -1*(r+l)/(r-l);
	out.m[13] = -1*(t+b)/(t-b);
	out.m[14] = n/(f-n);
	out.m[15] = 1;
	return out;
}

void CompileErrors(GLuint program){
		GLint success = 0;
		glGetShaderiv( program, GL_COMPILE_STATUS, &success);
		if (success == 1)
			return;
		GLint logSize = 0;
		glGetShaderiv(program, GL_INFO_LOG_LENGTH, &logSize);
		int loglen1;
		char logbuffer1[1000];
		glGetShaderInfoLog(program, sizeof(logbuffer1), &loglen1, logbuffer1);
		fprintf(stderr, "\n%.*s", loglen1, logbuffer1);
}

void ExitOnGLError(const char* error_message)
{
	const GLenum ErrorValue = glGetError();
	if (ErrorValue != GL_NO_ERROR)
	{
		std::cout << "ErrorValue = " << ErrorValue << "\n";
		const char* APPEND_DETAIL_STRING = ": %s\n";
		const size_t APPEND_LENGTH = strlen(APPEND_DETAIL_STRING) + 1;
		const size_t message_length = strlen(error_message);
		char* display_message = (char*)malloc(message_length + APPEND_LENGTH);

		memcpy(display_message, error_message, message_length);
		memcpy(&display_message[message_length], APPEND_DETAIL_STRING, APPEND_LENGTH);

		fprintf(stderr, display_message, gluErrorString(ErrorValue));

		free(display_message);
		exit(EXIT_FAILURE);
	}
}

GLuint LoadShader(const char* filename, GLenum shader_type)
{
	GLuint shader_id = 0;
	FILE* file;
	long file_size = -1;
	char* source;

	if (NULL != (file = fopen(filename, "rb")) &&
		0 == fseek(file, 0, SEEK_END) &&
		-1 != (file_size = ftell(file)))
	{
		rewind(file);
		
		if (NULL != (source = (char*)malloc(file_size + 1)))
		{
			if (file_size == (long)fread(source, sizeof(char), file_size, file))
			{
				source[file_size] = '\0';

				if (0 != (shader_id = glCreateShader(shader_type)))
				{
					const GLchar* glsl_source = source;
					glShaderSource(shader_id, 1, &glsl_source, NULL);
					glCompileShader(shader_id);
					ExitOnGLError("Could not compile a shader");
				}
				else
					fprintf(stderr, "ERROR: Could not create a shader.\n");
			}
			else
				fprintf(stderr, "ERROR: Could not read file %s\n", filename);

			free(source);
		}
		else
			fprintf(stderr, "ERROR: Could not allocate %i bytes.\n", file_size);

		fclose(file);
	}
	else
		fprintf(stderr, "ERROR: Could not open file %s\n", filename);
	CompileErrors(shader_id);
	return shader_id;
}

void CreateVector(Vector a, Vector b, Vector* res)
{
	res->v[0] = a.v[0] - b.v[0];
	res->v[1] = a.v[1] - b.v[1];
	res->v[2] = a.v[2] - b.v[2];
}

void CreateVector(Vertex a, Vertex b, Vector* res)
{
	res->v[0] = a.Position[0] - b.Position[0];
	res->v[1] = a.Position[1] - b.Position[1];
	res->v[2] = a.Position[2] - b.Position[2];
}

void CreateVector(Vertex a, Vector b, Vector* res)
{
	res->v[0] = a.Position[0] - b.v[0];
	res->v[1] = a.Position[1] - b.v[1];
	res->v[2] = a.Position[2] - b.v[2];
}

void Add(Vector a, Vector b, Vector* res)
{
	res->v[0] = a.v[0] + b.v[0];
	res->v[1] = a.v[1] + b.v[1];
	res->v[2] = a.v[2] + b.v[2];
}

float Dot(Vector a, Vector b)
{
	return (a.v[0]*b.v[0] + a.v[1]*b.v[1] + a.v[2]*b.v[2]);
}

void Cross(Vector a, Vector b, Vector* res)
{
	res->v[0] = a.v[1]*b.v[2] - a.v[2]*b.v[1];
	res->v[1] = a.v[2]*b.v[0] - a.v[0]*b.v[2];
	res->v[2] = a.v[0]*b.v[1] - a.v[1]*b.v[0];
	Normalize(res);
}

void Normalize(Vector* a)
{
	float len = (float) sqrt(a->v[0]*a->v[0] + a->v[1]*a->v[1] + a->v[2]*a->v[2]);
	a->v[0] = (len == 0? 0 : a->v[0] / len);
	a->v[1] = (len == 0? 0 : a->v[1] / len);
	a->v[2] = (len == 0? 0 : a->v[2] / len);
}

void CreateUp(Vector out, Vector up, Vector* res)
{
	float d = Dot(out, up);
	res->v[0] = up.v[0] - d * out.v[0];
	res->v[1] = up.v[1] - d * out.v[1];
	res->v[2] = up.v[2] - d * out.v[2];
}

void CreateColumnMajorMatrix(Vector row1, Vector row2, Vector row3, Matrix* out)
{
	Matrix rot = IDENTITY_MATRIX;
	rot.m[0] = row1.v[0];
	rot.m[4] = row1.v[1];
	rot.m[8] = row1.v[2];
	rot.m[1] = row2.v[0];
	rot.m[5] = row2.v[1];
	rot.m[9] = row2.v[2];
	rot.m[2] = row3.v[0];
	rot.m[6] = row3.v[1];
	rot.m[10] = row3.v[2];

	memcpy(out->m, rot.m, sizeof(out->m));
}

Matrix InverseMatrix(const Matrix* mat)
{
	Matrix res = IDENTITY_MATRIX;

	float m1 = (mat->m[5]* mat->m[10]) - (mat->m[9]* mat->m[6]);
	float m2 = (mat->m[4]* mat->m[10]) - (mat->m[8]* mat->m[6]);
	float m3 = (mat->m[4]* mat->m[9]) -  (mat->m[5]* mat->m[8]);
	float det = mat->m[0]*m1 - mat->m[1]*m2 + mat->m[2]*m3;

	res.m[0] = m1/det;
	res.m[4] = m2/det;
	res.m[8] = m3/det;
	res.m[1] = ((mat->m[1]* mat->m[10]) - (mat->m[9]* mat->m[2]))/det;
	res.m[5] = ((mat->m[0]* mat->m[10]) - (mat->m[8]* mat->m[2]))/det;
	res.m[9] = ((mat->m[0]* mat->m[9]) -  (mat->m[8]* mat->m[1]))/det;
	res.m[2] = ((mat->m[1]* mat->m[6]) -  (mat->m[5]* mat->m[2]))/det;
	res.m[6] = ((mat->m[0]* mat->m[6]) -  (mat->m[4]* mat->m[2]))/det;
   res.m[10] = ((mat->m[0]* mat->m[5]) -  (mat->m[4]* mat->m[1]))/det;

    return res;
}

Matrix TransposeMatrix(const Matrix* mat)
{
	Matrix res = IDENTITY_MATRIX;
	memcpy(res.m, mat->m, sizeof(mat->m));
	res.m[4] = mat->m[1];
	res.m[8] = mat->m[2];

	res.m[1] = mat->m[4];
	res.m[9] = mat->m[6];

	res.m[2] = mat->m[8];
	res.m[6] = mat->m[9];

	return res;
}

void CreateViewMatrix(Matrix* view, Vector eye, Vector lookAt)
{
	Vector out;
	CreateVector(lookAt, eye, &out);
	Normalize(&out);
	Vector right;
	Cross(UP, out, &right);
	Normalize(&right);
	Vector localUp;
	CreateUp(out, UP, &localUp);
	Normalize(&localUp);
	CreateColumnMajorMatrix(right, localUp, out, view);
}



void PrintVertex(Vertex v)
{
	printf("Position ");
	printf("%.6f,", v.Position[0] );
	printf("%.6f,", v.Position[1] );
	printf("%.6f  \n", v.Position[2] );

	printf("Color ");
	printf("%.2f,", v.Color[0] );
	printf("%.2f,", v.Color[0] );
	printf("%.2f  \n", v.Color[0] );
}

void PrintVector(Vector v)
{
	printf("Position ");
	printf("%f,", v.v[0] );
	printf("%f,", v.v[1] );
	printf("%f\n", v.v[2] );
	
}

void FindSurfaceVertex(Vertex v1, Vertex v2, Vertex v3, Vertex result)
{
	for(int i = 0; i < 3; i++)
		result.Position[i] = (v1.Position[i] + v2.Position[i])/2.0f;

	for(int i = 0; i < 3; i++)
		result.Position[i] = (result.Position[i] + v3.Position[i])/2.0f;
}