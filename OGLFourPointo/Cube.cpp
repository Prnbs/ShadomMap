#include "Cube.h"
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "objLoader.h"

#define DEPTH_TEXTURE_SIZE      4096

map<std::string, Edge> edgeTable;
map<std::string, Edge>::iterator it;
Vertex ShadowVertices[20000];
Vertex VERTICES[530];
Vector surfaceNorm[1024];
Vertex surfaceVertex[1024];
int countGlbl = 0;

void printVector(obj_vector *v)
{
	printf("%.2f,", v->e[0] );
	printf("%.2f,", v->e[1] );
	printf("%.2f  ", v->e[2] );
} 

void printVector(Vector *v)
{
	printf("%.2f,", v->v[0] );
	printf("%.2f,", v->v[1] );
	printf("%.2f  ", v->v[2] );
} 

void Cube::Create()
{
	ModelMatrix = IDENTITY_MATRIX;   
	TranslateMatrix(&ModelMatrix, 0.0f, 0.0f, 0.0f);
	//ScaleMatrix(&ModelMatrix, 0.01f, 0.01f, 0.01f);

	CubeRotation = 0;

	DirectLightColour.Color[0] = 0.0f;
	DirectLightColour.Color[1] = 0.5f;
	DirectLightColour.Color[2] = 0.0f;
	DirectLightColour.Color[3] = 0.1f;

	objLoader *objData = new objLoader();
	objData->load("..\\Models\\teapot.obj");
	
	Vector TexTea[530];
	float Color[4] = {0,0,0.0,1};
	Vector radius;
	for(int i=0; i<objData->vertexCount; i++)
	{
		VERTICES[i].Position[0] = 0.01f * (objData->vertexList[ i ])->e[0];
		VERTICES[i].Position[1] = 0.01f * (objData->vertexList[ i ])->e[1];
		VERTICES[i].Position[2] = 0.01f * (objData->vertexList[ i ])->e[2];
		VERTICES[i].Position[3] = 1.0f;
		VERTICES[i].Color[0] = Color[0];
		VERTICES[i].Color[1] = Color[1];
		VERTICES[i].Color[2] = Color[2];
		VERTICES[i].Color[3] = Color[3];  
		VERTICES[i].identity = i;

		FindMinAndMax(VERTICES[i].Position[0], 
				VERTICES[i].Position[1], VERTICES[i].Position[2]);
	
	}

	Xc = (Xmin + Xmax) / 2.0f; 
	Zc = (Zmin + Zmax) / 2.0f;
	float theta;
	float X, Z;
	for(int i=0; i<objData->vertexCount; i++)
	{
		X = (VERTICES[i].Position[0] - Xc);
		Z = (VERTICES[i].Position[2] - Zc);
		if(( X > 0 && Z > 0) || (Z > 0 && X < 0))
			theta = atan2(Z, X);
		if((X < 0 && Z < 0) || (X > 0 && Z < 0))
			theta = (2 * 3.14159f) - atan2(-1*Z, X);

		TexTea[i].v[0] = theta/(2 * 3.14159f);
		TexTea[i].v[1] = (VERTICES[i].Position[1] - Ymax) / (Ymin - Ymax);

	}

	int ctr = 0;
	//cout << objData->faceCount << endl;
	for(int i=0; i<objData->faceCount; i++)
	{
		obj_face *o = objData->faceList[i];
		for(int j=0; j<3; j++)
		{
			INDICES[ctr++] = o->vertex_index[j];
		}
	} 
	//calculate surface normal
	Vertex p1, p2, p3;
	
	Vector a, b;
	
	for(int i = 0; i < ctr; i+=3)
	{
		p1 = VERTICES[INDICES[i]];
		p2 = VERTICES[INDICES[i+1]];
		p3 = VERTICES[INDICES[i+2]];
		
		CreateVector(p1,p2,&a);
		CreateVector(p2,p3,&b);
		Cross(a, b, &surfaceNorm[countGlbl]);
		FindSurfaceVertex(p1, p2, p3, surfaceVertex[countGlbl++]);
	}
	cout << "Surf count = " << countGlbl << endl;
	Vector Normals[530];
	
	for(int i=0; i<objData->normalCount; i++)
	{
		Normals[i].v[0] = (objData->normalList[i])->e[0];
		Normals[i].v[1] = (objData->normalList[i])->e[1];
		Normals[i].v[2] = (objData->normalList[i])->e[2];
	}
	
	//basic drawing
	ShaderCubeIds.push_back(glCreateProgram());
	//cout << glCreateProgram()<< endl;
	ExitOnGLError("ERROR: Could not create the shader program");
	
	ShaderCubeIds.push_back(LoadShader("..\\Shaders\\Texture.fragment.glsl", GL_FRAGMENT_SHADER));
	ShaderCubeIds.push_back(LoadShader("..\\Shaders\\Texture.vertex.glsl", GL_VERTEX_SHADER));
	
	glAttachShader(ShaderCubeIds[0], ShaderCubeIds[1]);
	cout << ShaderCubeIds.at(0) << endl;
	cout << ShaderCubeIds.at(1) << endl;
	cout << ShaderCubeIds.at(2) << endl;
	ExitOnGLError("ERROR: Could not glAttachShader fragment shader program");
	glAttachShader(ShaderCubeIds[0], ShaderCubeIds[2]);
	ExitOnGLError("ERROR: Could not glAttachShader vertex shader program");
	
	glLinkProgram(ShaderCubeIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program");

	ModelMatrixUniformLocation = glGetUniformLocation(ShaderCubeIds[0], "ModelMatrix");
	ExitOnGLError("ERROR: Could not get shadow shader uniform locations -- ModelMatrix");
	ViewMatrixUniformLocation = glGetUniformLocation(ShaderCubeIds[0], "ViewMatrix");
	ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderCubeIds[0], "ProjectionMatrix");
	DirectionLightUniformLocation = glGetUniformLocation(ShaderCubeIds[0], "DirectionLight");
	DirectionLightColourUniformLocation = glGetUniformLocation(ShaderCubeIds[0], "DirectionLightColour");
	ViewVectorUniformLocation = glGetUniformLocation(ShaderCubeIds[0], "ViewVector");
	MVMatrixUniformLocation = glGetUniformLocation(ShaderCubeIds[0], "MVMatrix");
	ExitOnGLError("ERROR: Could not get shadow shader uniform locations -- MVMatrix");
	
	gaussianTextureUnif = glGetUniformLocation(ShaderCubeIds[0], "gaussianTexture");
	bumpTextureUnif = glGetUniformLocation(ShaderCubeIds[0], "bumpTexture");

	ExitOnGLError("ERROR: Could not get shader uniform locations -- ");

	//Shadow mapping
	/*state.ShaderShadowIds[0] = glCreateProgram();
	ExitOnGLError("ERROR: Could not create the shadow shader program");
	
	state.ShaderShadowIds[1] = LoadShader("..\\Shaders\\ShadowMap.fragment.glsl", GL_FRAGMENT_SHADER);
	state.ShaderShadowIds[2] = LoadShader("..\\Shaders\\ShadowMap.vertex.glsl", GL_VERTEX_SHADER);
	glAttachShader(state.ShaderShadowIds[0], state.ShaderShadowIds[1]);
	ExitOnGLError("ERROR: Could not glAttachShader fragment shader program");
	glAttachShader(state.ShaderShadowIds[0], state.ShaderShadowIds[2]);
	ExitOnGLError("ERROR: Could not glAttachShader vertex shader program");
	
	glLinkProgram(state.ShaderShadowIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program");

	ShadowModelMatrixUniformLocation = glGetUniformLocation(state.ShaderShadowIds[0], "ShadowModelMatrix");
	ExitOnGLError("ERROR: Could not get shadow shader uniform locations -- ShadowModelMatrix");
	ShadowViewMatrixUniformLocation = glGetUniformLocation(state.ShaderShadowIds[0], "ShadowViewMatrix");
	ExitOnGLError("ERROR: Could not get shadow shader uniform locations -- ShadowViewMatrix");
	OrthoProjectionMatrixUniformLocation = glGetUniformLocation(state.ShaderShadowIds[0], "OrthoProjectionMatrix");
	ExitOnGLError("ERROR: Could not get shadow shader uniform locations -- OrthoProjectionMatrixUniformLocation"); */

	glGenFramebuffers(1, &state.shadow_Fbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, state.shadow_Fbuffer);

	glGenTextures(1, &state.shadow_tex);
	ExitOnGLError("ERROR: Could not gen glGenTextures for shadow shader "); 
	glBindTexture(GL_TEXTURE_2D, state.shadow_tex);
	ExitOnGLError("ERROR: Could not bind glGenTextures for shadow shader "); 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_NOTEQUAL);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, state.shadow_tex, 0);

	glGenTextures(1, &state.debug_shadow_tex);
    glBindTexture(GL_TEXTURE_2D, state.debug_shadow_tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, state.debug_shadow_tex, 0);

	if (GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER))
    {
        printf("FBO %d set up successfully. Yay!\n", state.shadow_Fbuffer);
    }
    else
    {
        printf("FBO %d NOT set up properly!\n", state.shadow_Fbuffer);
    }
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenVertexArrays(1, &BufferIds[0]);  
	ExitOnGLError("ERROR: Could not generate the VAO");
	glBindVertexArray(BufferIds[0]); 
	ExitOnGLError("ERROR: Could not bind the VAO");

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
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

	glGenBuffers(1, &TexBuffer);
	ExitOnGLError("ERROR: Could not generate the texture buffer");
	glBindBuffer(GL_ARRAY_BUFFER, TexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexTea), TexTea, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the texture to the VAO");
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(TexTea[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");
	
	glBindVertexArray(0);
	string filename = "..\\Textures\\horse.png";
	string bumpyFilename = "..\\Textures\\normal.jpg";
	glGenSamplers(1, &teaSampler);
	SetTexParams(filename, teaTex, teaSampler, 0);
	
	ExitOnGLError("ERROR: Could not gen teasampler sampler");
		   glSamplerParameteri(teaSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glSamplerParameteri(teaSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		   glSamplerParameteri(teaSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		   glSamplerParameteri(teaSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ExitOnGLError("ERROR: Could not load teasampler samplers attributes");
	glGenSamplers(1, &bumpSampler);
	SetTexParams(bumpyFilename, bumpTex, bumpSampler, 1);
	
	ExitOnGLError("ERROR: Could not gen bump sampler");
		   glSamplerParameteri(bumpSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glSamplerParameteri(bumpSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		   glSamplerParameteri(bumpSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		   glSamplerParameteri(bumpSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	ExitOnGLError("ERROR: Could not load bump sampler's samplers attributes");

	
}

void Cube::SetTexParams(string filename, GLuint &imageHandle, GLuint &sampler, GLuint offset)
{
	GLuint texId;
	ilInit(); /* Initialization of DevIL */
	ilGenImages(1, &texid); /* Generation of one image name */
	ilBindImage(texid); /* Binding of image name */
	
	bool  success = ilLoadImage((const ILstring)filename.c_str()); /* Loading of image "image.jpg" */

    if (success) /* If no error occured: */
	{
	   cout << "\n Loaded image\n";
	   success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); /* Convert every colour component into
		 unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */

	   glGenTextures(1, &imageHandle); /* Texture name generation */
	   glActiveTexture(GL_TEXTURE0+offset);
	   glBindTexture(GL_TEXTURE_2D, imageHandle); /* Binding of texture name */

	   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	  
	   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
		 ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
		 ilGetData()); /* Texture specification */
	  
	   glBindSampler(0, sampler);
	   ilDeleteImages(1, &texid);
	}
	else
		cout << "\n Failed to load image\n";
}

void Cube::FindMinAndMax(float x, float y, float z)
{
	if ( x > Xmax)
		Xmax = x;
	if ( x < Xmin)
		Xmin = x;
	if ( y > Ymax)
		Ymax = y;
	if ( y < Ymin)
		Ymin = y;
	if ( z > Zmax)
		Zmax = z;
	if ( z < Zmin)
		Zmin = z;
}


void Cube::Destroy()
{
	glDetachShader(ShaderCubeIds[0], ShaderCubeIds[1]);
	glDetachShader(ShaderCubeIds[0], ShaderCubeIds[2]);
	glDeleteShader(ShaderCubeIds[1]);
	glDeleteShader(ShaderCubeIds[2]);
	glDeleteProgram(ShaderCubeIds[0]);
	ExitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(1, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");


	/*glDetachShader(state.ShaderShadowIds[0],state.ShaderShadowIds[1]);
	glDetachShader(state.ShaderShadowIds[0],state.ShaderShadowIds[2]);
	glDeleteShader(state.ShaderShadowIds[1]);
	glDeleteShader(state.ShaderShadowIds[2]);
	glDeleteProgram(state.ShaderShadowIds[0]);
	ExitOnGLError("ERROR: Could not destroy the shaders");*/

	glDeleteBuffers(1, &ShadowBufferIds[1]);
	glDeleteVertexArrays(1, &ShadowBufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

void Cube::DrawFromLightPOV()
{
	glUseProgram(state.ShaderShadowIds[0]);

	ExitOnGLError("ERROR: Could not use the shadow shader program in DrawFromLightPOV cube");
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
//	glBindFramebuffer(GL_FRAMEBUFFER, state.shadow_Fbuffer);
//	glBindTexture(GL_TEXTURE_2D, state.shadow_tex);
	//cout << state.shadow_Fbuffer << endl;
	glUniformMatrix4fv(ShadowModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
	//glUniformMatrix4fv(ShadowViewMatrixUniformLocation, 1, GL_FALSE, state.GetShadowViewMatrix().m);
    glUniformMatrix4fv(ShadowViewMatrixUniformLocation, 1, GL_FALSE,state.GetShadowViewMatrix().m);
    
	glUniformMatrix4fv(OrthoProjectionMatrixUniformLocation, 1, GL_FALSE, state.GetOrthoProjectionMatrix().m);
	
  	ExitOnGLError("ERROR: Could not set the shader uniforms");

	glBindVertexArray(BufferIds[0]);
	ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");
    glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDrawElements(GL_TRIANGLES, sizeof(INDICES), GL_UNSIGNED_INT, (GLvoid*)0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	ExitOnGLError("ERROR: Could not draw the cube");
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Cube::Draw(GLboolean disableColorWrite)
{
	static const GLfloat ones[] = { 1.0f };
    static const GLfloat zero[] = { 0.0f };
	glUseProgram((GLuint)ShaderCubeIds.at(0));
		ExitOnGLError("ERROR: Could not use the cube shader program");

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);

		ViewVect.v[0] = state.GetViewMatrix().m[2];
		ViewVect.v[1] = state.GetViewMatrix().m[6];
		ViewVect.v[2] = state.GetViewMatrix().m[10];

		glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
		glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, state.GetViewMatrix().m);

		glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, state.GetProjectionMatrix().m);
		glUniform3fv(DirectionLightUniformLocation, 1, (state.GetLightDirection()).v);
		glUniform4fv(DirectionLightColourUniformLocation, 1, DirectLightColour.Color);
		glUniform3fv(ViewVectorUniformLocation, 1, ViewVect.v);
		glUniform1i(gaussianTextureUnif, 0);
		glUniform1i(bumpTextureUnif, 1);
  
		Matrix MV = MultiplyMatrices(&(state.GetViewMatrix()), &ModelMatrix);
		Matrix MVInv = TransposeMatrix(&(InverseMatrix(&MV)));
		glUniformMatrix4fv(MVMatrixUniformLocation, 1, GL_FALSE, MVInv.m);
		ExitOnGLError("ERROR: Could not set the shader uniforms");

		glClearBufferfv(GL_DEPTH, 0, ones);

		glBindVertexArray(BufferIds[0]);
			ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, teaTex); /* Binding of texture name */
			glBindSampler(0, teaSampler);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, bumpTex); /* Binding of texture name */
			glBindSampler(0, bumpSampler);
            glColorMask(disableColorWrite,disableColorWrite,disableColorWrite,disableColorWrite);
			glDrawElements(GL_TRIANGLES, sizeof(INDICES), GL_UNSIGNED_INT, (GLvoid*)0);
			ExitOnGLError("ERROR: Could not draw the cube");
	
		//	glBindTexture(GL_TEXTURE_2D, 0); /* Binding of texture name */
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
	return ShaderCubeIds[0];
}

