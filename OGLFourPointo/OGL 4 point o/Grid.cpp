#include "Grid.h"
#include "glext.h"

void Grid::Create()
{
	
	DirectLight.v[0] = 10.0f;
	DirectLight.v[1] = 10.0f;
	DirectLight.v[2] = 10.0f;

	DirectLightColour.Color[0] = 0.9f;
	DirectLightColour.Color[1] = 0.0f;
	DirectLightColour.Color[2] = 0.0f;
	DirectLightColour.Color[3] = 0.1f;

	Vertex Grid[(SEGMENT+1) * (SEGMENT+1)];
	int n = 0;
	for(int i = 0; i <= SEGMENT; i++)
	{
		for(int j = 0; j <= SEGMENT; j++)
		{
			Grid[n].Position[0] = (float)i - SEGMENT/2;
			Grid[n].Position[1] = -1.0f;
			Grid[n].Position[2] = (float)j - SEGMENT/2;
			Grid[n].Position[3] = 1.0f;
		
			Grid[n].Color[0] = 1.0f;
			Grid[n].Color[1] = 0.0f;
			Grid[n].Color[2] = 0.0f;
			Grid[n++].Color[3] = 1.0f;
		}
	}
	cout << "N = " << n << endl;

	ShaderIds[0] = glCreateProgram();
	ExitOnGLError("ERROR: Could not create the shader program");
	{
		ShaderIds[1] = LoadShader("Texture.fragment.glsl", GL_FRAGMENT_SHADER);
		ShaderIds[2] = LoadShader("Texture.vertex.glsl", GL_VERTEX_SHADER);
		glAttachShader(ShaderIds[0], ShaderIds[1]);
		glAttachShader(ShaderIds[0], ShaderIds[2]);
	}
	glLinkProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program");

	ModelMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
	DirectionLightUniformLocation = glGetUniformLocation(ShaderIds[0], "DirectionLight");
	DirectionLightColourUniformLocation = glGetUniformLocation(ShaderIds[0], "DirectionLightColour");
	ViewVectorUniformLocation = glGetUniformLocation(ShaderIds[0], "ViewVector");
	MVMatrixUniformLocation = glGetUniformLocation(ShaderIds[0], "MVMatrix");

	ExitOnGLError("ERROR: Could not get shader uniform locations");

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Grid), Grid, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the VBO to the VAO");

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Grid[0]), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Grid[0]), (GLvoid*)sizeof(Grid[0].Position));
	ExitOnGLError("ERROR: Could not set VAO attributes");

	int k = 0;
	for(GLushort i = 0; i < SEGMENT; i++)
	{
		for(GLushort j = 0;j < SEGMENT; j++)
		{
			indices[k++] = i * (SEGMENT+1) + j;
			indices[k++] = i * (SEGMENT+1) + j + 1;
			indices[k++] = (i + 1) * (SEGMENT+1) + j;

			indices[k++] = i * (SEGMENT+1) + j + 1;
			indices[k++] = (i + 1) * (SEGMENT+1) + j + 1;
			indices[k++] = (i + 1) * (SEGMENT+1) + j;
		}
	}

	Vector Normals[(SEGMENT+1) * (SEGMENT+1)];// = {{0,1,0}};
	for(int i = 0; i < (SEGMENT+1) * (SEGMENT+1); i++)
	{
		Normals[i].v[0] = 0.0;
		Normals[i].v[1] = 10.0;
		Normals[i].v[2] = 0.0;
	}

	cout << "Index size = " << k << endl;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

	glGenBuffers(1, &VertNormBuff);
	ExitOnGLError("ERROR: Could not generate the vertex normal buffer");

	glBindBuffer(GL_ARRAY_BUFFER, VertNormBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), Normals, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the Vertex Normals to the VAO");
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Normals[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glBindVertexArray(0);

	CreateGaussianTextures();
}

int Grid::CalcCosAngResolution(int level)
{
	const int cosAngleStart = 64;
	return cosAngleStart * ((int)pow(2.0f, level));
}


GLuint Grid::CreateGaussianTexture(int cosAngleResolution)
{
	std::vector<GLubyte> textureData;
	BuildGaussianData(textureData, cosAngleResolution);

	GLuint gaussTexture;
	glGenTextures(1, &gaussTexture);
	glBindTexture(GL_TEXTURE_1D, gaussTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R8, cosAngleResolution, 0,
		GL_RED, GL_UNSIGNED_BYTE, &textureData[0]);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);
	glBindTexture(GL_TEXTURE_1D, 0);

	return gaussTexture;
}

void Grid::CreateGaussianTextures()
{
	for(int loop = 0; loop < NUM_GAUSS_TEXTURES; loop++)
	{
		int cosAngleResolution = CalcCosAngResolution(loop);
		g_gaussTextures[loop] = CreateGaussianTexture(cosAngleResolution);
	}

	glGenSamplers(1, &g_gaussSampler);
	glSamplerParameteri(g_gaussSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(g_gaussSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(g_gaussSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}


void Grid::BuildGaussianData(vector<GLubyte> &textureData, int cosAngleResolution)
{
    textureData.resize(cosAngleResolution);

    vector<GLubyte>::iterator currIt = textureData.begin();
    for(int iCosAng = 0; iCosAng < cosAngleResolution; iCosAng++)
    {
        float cosAng = iCosAng / (float)(cosAngleResolution - 1);
        float angle = acosf(cosAng);
        float exponent = angle / 34.0f;
        exponent = -(exponent * exponent);
        float gaussianTerm = exp(exponent);
        
        *currIt++ = (GLubyte)(gaussianTerm * 255.0f);
    }
}

void Grid::Destroy()
{
	glDetachShader(ShaderIds[0], ShaderIds[1]);
	glDetachShader(ShaderIds[0], ShaderIds[2]);
	glDeleteShader(ShaderIds[1]);
	glDeleteShader(ShaderIds[2]);
	glDeleteProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Coul d not destroy the shaders");

	glDeleteBuffers(2, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");

}

void Grid::Draw()
{
	ModelMatrix = IDENTITY_MATRIX;
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glUseProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not use the shader program");

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

	glDrawElementsInstancedARB(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_SHORT, (GLvoid*)0, 2);
	ExitOnGLError("ERROR: Could not draw the cube");

	glBindVertexArray(0);
	glUseProgram(0);

}

GLuint Grid::GetViewMatrixUniformLocation()
{
	return ViewMatrixUniformLocation;
}

GLuint Grid::GetProjectionMatrixUniformLocation()
{
	return ProjectionMatrixUniformLocation;
}

GLuint Grid::GetProgramId()
{
	return ShaderIds[0];
}
