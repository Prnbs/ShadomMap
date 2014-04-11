#include "Grid.h"
#include <cstring>

void Grid::Create()
{
	
	DirectLightColour.Color[0] = 0.9f;
	DirectLightColour.Color[1] = 0.0f;
	DirectLightColour.Color[2] = 0.0f;
	DirectLightColour.Color[3] = 0.1f;

	Vertex Grid[(SEGMENT+1) * (SEGMENT+1)]; 
	Vector Tex[(SEGMENT+1) * (SEGMENT+1)];
	int n = 0;
	for(int i = 0; i <= SEGMENT; i++)
	{
		for(int j = 0; j <= SEGMENT; j++)
		{
			Grid[n].Position[0] = (float)i - SEGMENT/2;
			Grid[n].Position[1] = -1.0f;
			Grid[n].Position[2] = (float)j - SEGMENT/2;
			Grid[n].Position[3] = 1.0f;
			//cout << "X = " << (float)i / (float)SEGMENT << ", Y = " << (float)j / (float)SEGMENT << endl;
		
			Grid[n].Color[0] = 1.0f;
			Grid[n].Color[1] = 0.0f;
			Grid[n].Color[2] = 0.0f;
			Grid[n++].Color[3] = 1.0f;

			Tex[n].v[0] =  (float)i / (float)SEGMENT;
			Tex[n].v[1] = (float)j / (float)SEGMENT;
		
		}
	}
	cout << "N = " << n << endl;

	ShaderGridIds.push_back(glCreateProgram());
	ExitOnGLError("ERROR: Could not create the shader program- Grid basic draw");
	
		ShaderGridIds.push_back(LoadShader("Phong.fragment.glsl", GL_FRAGMENT_SHADER));
		ShaderGridIds.push_back(LoadShader("Phong.vertex.glsl", GL_VERTEX_SHADER));
		glAttachShader(ShaderGridIds.at(0), ShaderGridIds.at(1));
		glAttachShader(ShaderGridIds.at(0), ShaderGridIds.at(2));

		ExitOnGLError("ERROR: Could not glAttachShader the shader program");

	glLinkProgram(ShaderGridIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program");
	
	ModelMatrixUniformLocation = glGetUniformLocation(ShaderGridIds.at(0), "ModelMatrix");
	ExitOnGLError("ERROR: Could not get ModelMatrix uniform locations -- gRID");

	ViewMatrixUniformLocation = glGetUniformLocation(ShaderGridIds.at(0), "ViewMatrix");
	ExitOnGLError("ERROR: Could not get ViewMatrix uniform locations -- gRID");

	ProjectionMatrixUniformLocation = glGetUniformLocation(ShaderGridIds.at(0), "ProjectionMatrix");
	ExitOnGLError("ERROR: Could not get ProjectionMatrix uniform locations -- gRID");

	DirectionLightUniformLocation = glGetUniformLocation(ShaderGridIds.at(0), "DirectionLight");
	ExitOnGLError("ERROR: Could not get DirectionLight uniform locations -- gRID");

	DirectionLightColourUniformLocation = glGetUniformLocation(ShaderGridIds.at(0), "DirectionLightColour");
	ExitOnGLError("ERROR: Could not get DirectionLightColour uniform locations -- gRID");

	ViewVectorUniformLocation = glGetUniformLocation(ShaderGridIds.at(0), "ViewVector");
	ExitOnGLError("ERROR: Could not get ViewVector uniform locations -- gRID");

	MVMatrixUniformLocation = glGetUniformLocation(ShaderGridIds.at(0), "MVMatrix");
	ExitOnGLError("ERROR: Could not get MVMatrix uniform locations -- gRID");

	ShadowBiasMatrixUniformLocation = glGetUniformLocation(ShaderGridIds.at(0), "ShadowBias");
	ExitOnGLError("ERROR: Could not get ShadowBias uniform locations -- gRID");

	gaussianTextureUnif = glGetUniformLocation(ShaderGridIds.at(0), "gaussianTexture");
	ExitOnGLError("ERROR: Could not get gaussianTexture uniform locations -- gRID");

	ShadowMapUniformSampler =  glGetUniformLocation(ShaderGridIds.at(0), "shadow_tex");
	ExitOnGLError("ERROR: Could not get shadow_tex uniform locations -- gRID");

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

	glGenBuffers(1, &TexBuffer);
	ExitOnGLError("ERROR: Could not generate the texture buffer");
	glBindBuffer(GL_ARRAY_BUFFER, TexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tex), Tex, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the texture to the VAO");
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Tex[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glBindVertexArray(0);

	string filename = "C:\\Users\\Public\\Pictures\\Sample Pictures\\jellyfish.jpg";
	glGenSamplers(1, &g_gaussSampler);
	SetTexParams(filename, image, g_gaussSampler, 0);
	
		   glSamplerParameteri(g_gaussSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glSamplerParameteri(g_gaussSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		   glSamplerParameteri(g_gaussSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		   glSamplerParameteri(g_gaussSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenSamplers(1, &d_shadowSampler);
		   glSamplerParameteri(d_shadowSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glSamplerParameteri(d_shadowSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		   glSamplerParameteri(d_shadowSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		   glSamplerParameteri(d_shadowSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Grid::SetTexParams(string filename, GLuint &imageHandle, GLuint &sampler, GLuint offset)
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

void Grid::Destroy()
{
	glDetachShader(ShaderGridIds.at(0), ShaderGridIds.at(1));
	glDetachShader(ShaderGridIds.at(0), ShaderGridIds.at(2));
	glDeleteShader(ShaderGridIds.at(1));
	glDeleteShader(ShaderGridIds.at(2));
	glDeleteProgram(ShaderGridIds.at(0));
	ExitOnGLError("ERROR: Coul d not destroy the shaders");

	glDeleteBuffers(2, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");

}

void Grid::DrawFromLightPOV()
{
	return;
	ModelMatrix = IDENTITY_MATRIX;
	

	glUseProgram(state.ShaderShadowIds[0]);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glBindFramebuffer(GL_FRAMEBUFFER, state.shadow_Fbuffer);
	glBindTexture(GL_TEXTURE_2D, state.shadow_tex);

	ExitOnGLError("ERROR: Could not use the shader program grid shadow");

	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, state.GetShadowViewMatrix().m);
	glUniformMatrix4fv(OrthoProjectionMatrixUniformLocation, 1, GL_FALSE, state.GetOrthoProjectionMatrix().m);
	

	glBindVertexArray(BufferIds[0]);
	ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_SHORT, (GLvoid*)0);
	
	ExitOnGLError("ERROR: Could not draw the grid");
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	//glUseProgram(0);

}

void Grid::Draw()
{
	ModelMatrix = IDENTITY_MATRIX;
	
	//glEnable(GL_CULL_FACE);
	ExitOnGLError("ERROR:Something wrong in cube's draw if I get here");
	//glCullFace(GL_BACK);
	//ExitOnGLError("ERROR: Could not cull back faces in grid");
	glFrontFace(GL_CCW);
	ExitOnGLError("ERROR: Could not cull front faces in grid");
	//glEnable(GL_STENCIL_TEST);
	
	glUseProgram(ShaderGridIds.at(0));
	ExitOnGLError("ERROR: Could not use the shader program grid draw --------------------->");

	glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
	glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, state.GetViewMatrix().m);
	glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, state.GetProjectionMatrix().m);
	glUniform3fv(DirectionLightUniformLocation, 1, (state.GetLightDirection()).v);
	glUniform4fv(DirectionLightColourUniformLocation, 1, DirectLightColour.Color);
	glUniform3fv(ViewVectorUniformLocation, 1, ViewVect.v);
	glUniform1i(gaussianTextureUnif, 0);
	glUniform1i(ShadowMapUniformSampler,1);
	glUniformMatrix4fv(ShadowBiasMatrixUniformLocation, 1, GL_FALSE, state.GetShadowBiasMatrix(ModelMatrix).m);
  
	Matrix MV = MultiplyMatrices(&(state.GetViewMatrix()), &ModelMatrix);
	Matrix MVInv = TransposeMatrix(&(InverseMatrix(&MV)));
	glUniformMatrix4fv(MVMatrixUniformLocation, 1, GL_FALSE, MVInv.m);
	ExitOnGLError("ERROR: Could not set the shader uniforms -- Grid");

	glBindVertexArray(BufferIds[0]);
	ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image); /* Binding of texture name */
	glBindSampler(0, g_gaussSampler);
	ExitOnGLError("ERROR: Could not bind grid samplers");

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, state.shadow_tex); /* Binding of texture name */
	glBindSampler(1, d_shadowSampler);
	
	
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_SHORT, (GLvoid*)0);
	
	ExitOnGLError("ERROR: Could not draw the grid");
	glBindTexture(GL_TEXTURE_2D, 0);
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
	return ShaderGridIds.at(0);
}
