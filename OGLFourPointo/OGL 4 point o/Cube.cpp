#include "Cube.h"
#include <iostream>
#include "objLoader.h"

void printVector(obj_vector *v)
{
	printf("%.2f,", v->e[0] );
	printf("%.2f,", v->e[1] );
	printf("%.2f  ", v->e[2] );
} 

void Cube::Create()
{
	ModelMatrix = IDENTITY_MATRIX;   
	TranslateMatrix(&ModelMatrix, 0.0f, 0.0f, 0.0f);
	ScaleMatrix(&ModelMatrix, 0.01f, 0.01f, 0.01f);

	CubeRotation = 0;
	
	DirectLight.v[0] = 10.0f;
	DirectLight.v[1] = 10.0f;
	DirectLight.v[2] = 10.0f;

	DirectLightColour.Color[0] = 0.9f;
	DirectLightColour.Color[1] = 0.0f;
	DirectLightColour.Color[2] = 0.0f;
	DirectLightColour.Color[3] = 0.1f;

	objLoader *objData = new objLoader();
	objData->load("teapot.obj");
	
	Vertex VERTICES[530];
	float Color[4] = {1,1,0,1};

	for(int i=0; i<objData->vertexCount; i++)
	{
		VERTICES[i].Position[0] = (objData->vertexList[ i ])->e[0];
		VERTICES[i].Position[1] = (objData->vertexList[ i ])->e[1];
		VERTICES[i].Position[2] = (objData->vertexList[ i ])->e[2];
		VERTICES[i].Position[3] = 1.0f;
		VERTICES[i].Color[0] = Color[0];
		VERTICES[i].Color[1] = Color[1];
		VERTICES[i].Color[2] = Color[2];
		VERTICES[i].Color[3] = Color[3];  
	}
	
	int ctr = 0;
	for(int i=0; i<objData->faceCount; i++)
	{
		obj_face *o = objData->faceList[i];
		
		for(int j=0; j<3; j++)
		{
			INDICES[ctr++] = o->vertex_index[j];
		}
	}

	Vector Normals[530];
	for(int i=0; i<objData->normalCount; i++)
	{
		Normals[i].v[0] = (objData->normalList[i])->e[0];
		Normals[i].v[1] = (objData->normalList[i])->e[1];
		Normals[i].v[2] = (objData->normalList[i])->e[2];
	}

	ShaderIds[0] = glCreateProgram();
	ExitOnGLError("ERROR: Could not create the shader program");
	
	ShaderIds[1] = LoadShader("Phong.fragment.glsl", GL_FRAGMENT_SHADER);
	ShaderIds[2] = LoadShader("Phong.vertex.glsl", GL_VERTEX_SHADER);
	glAttachShader(ShaderIds[0], ShaderIds[1]);
	glAttachShader(ShaderIds[0], ShaderIds[2]);
	
	glLinkProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program");

	ModelMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
	DirectionLightUniformLocation = glGetUniformLocation(ShaderIds[0], "DirectionLight");
	DirectionLightColourUniformLocation = glGetUniformLocation(ShaderIds[0], "DirectionLightColour");
	ViewVectorUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewVector");
	MVMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "MVMatrix");

	ExitOnGLError("ERROR: Could not get shader uniform locations -- ");

	glGenVertexArrays(1, &BufferIds[0]);  
	ExitOnGLError("ERROR: Could not generate the VAO");
	glBindVertexArray(BufferIds[0]); 
	ExitOnGLError("ERROR: Could not bind the VAO");

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	glGenBuffers(2, &BufferIds[1]);
	ExitOnGLError("ERROR: Could not generate the buffer objects");

	glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the VBO to the VAO");
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the IBO to the VAO");
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].Position));
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glGenBuffers(1, &VertNormBuff);
	ExitOnGLError("ERROR: Could not generate the vertex normal buffer");

	glBindBuffer(GL_ARRAY_BUFFER, VertNormBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), Normals, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the Vertex Normals to the VAO");
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Normals[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");
	
	glBindVertexArray(0);
}

void Cube::Destroy()
{
	glDetachShader(ShaderIds[0], ShaderIds[1]);
	glDetachShader(ShaderIds[0], ShaderIds[2]);
	glDeleteShader(ShaderIds[1]);
	glDeleteShader(ShaderIds[2]);
	glDeleteProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(2, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

void Cube::Draw()
{
	/*float CubeAngle;
	clock_t Now = clock();
	if (state.GetLastTime() == 0)
		state.SetLastTime(Now);
	CubeRotation += 45.0f * (float)(Now - state.GetLastTime()) / CLOCKS_PER_SEC;
	CubeRotation = 0;
	CubeAngle = DegreesToRadians(CubeRotation);
	state.SetLastTime(Now);*/ 

	glUseProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not use the shader program");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	ViewVect.v[0] = state.GetViewMatrix().m[2];
	ViewVect.v[1] = state.GetViewMatrix().m[6];
	ViewVect.v[2] = state.GetViewMatrix().m[10];

	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, state.GetViewMatrix().m);
	glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, state.GetProjectionMatrix().m);
	glUniform3fv(DirectionLightUniformLocation, 1, DirectLight.v);
	glUniform4fv(DirectionLightColourUniformLocation, 1, DirectLightColour.Color);
	glUniform3fv(ViewVectorUniformLocation, 1, ViewVect.v);
  
	Matrix MV = MultiplyMatrices(&(state.GetViewMatrix()), &ModelMatrix);
	Matrix MVInv = TransposeMatrix(&(InverseMatrix(&MV)));
	glUniformMatrix4fv(MVMatrixUniformLocation, 1, GL_FALSE, MVInv.m);
	ExitOnGLError("ERROR: Could not set the shader uniforms");

	glBindVertexArray(BufferIds[0]);
	ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

	glDrawElements(GL_TRIANGLES, sizeof(INDICES), GL_UNSIGNED_INT, (GLvoid*)0);
	ExitOnGLError("ERROR: Could not draw the cube");

	glBindVertexArray(0);
	glUseProgram(0);
}

GLuint Cube::GetViewMatrixUniformLocation()
{
	return ViewMatrixUniformLocation;
}

GLuint Cube::GetProjectionMatrixUniformLocation()
{
	return ProjectionMatrixUniformLocation;
}

GLuint Cube::GetProgramId()
{
	return ShaderIds[0];
}

