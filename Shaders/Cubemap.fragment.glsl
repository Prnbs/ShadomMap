#version 400

in vec3 ReflectDir;

out vec4 out_Color;

uniform samplerCube CubeMapTex;
  
void main(void)
{
	vec3 cubeMapColor = textureCube(CubeMapTex,ReflectDir);
	
	out_Color =  vec4(cubeMapColor,1.0) ;
}