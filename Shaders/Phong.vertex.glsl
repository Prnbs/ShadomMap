#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
layout(location=2) in vec3 in_Vertex_Normal;
layout(location=3) in vec3 in_Tex_Posn;
out vec3 FragNormal;
out vec4 FragPosition;
out vec3 DirectionLightEye;
out vec3 TexPosn;
out VS_OUT
{
	vec4 shadow_coord;
}vs_out;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVMatrix;
uniform vec3 DirectionLight;
uniform mat4 ShadowBias;


void main(void)
{
	FragNormal = vec3(MVMatrix * vec4(in_Vertex_Normal, 0.0));
	FragPosition =  vec4(ViewMatrix * ModelMatrix  * in_Position);
	TexPosn = vec3(ModelMatrix  * vec4(in_Tex_Posn, 0.0));
	DirectionLightEye = vec3(ViewMatrix * vec4(DirectionLight, 0.0));
	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * in_Position;
	vs_out.shadow_coord = (ProjectionMatrix * ShadowBias * ModelMatrix) * in_Position;
    //vs_out.shadow_coord = in_Position;
} 

