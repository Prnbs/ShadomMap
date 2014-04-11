#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
layout(location=2) in vec3 in_Vertex_Normal;
layout(location=3) in vec3 in_Tex_Posn;
out vec3 ReflectDir;
vec3 FragNormal;
vec4 FragPosition;
//vec3 DirectionLightEye;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVMatrix;
//uniform vec3 DirectionLight;
uniform vec3 WorldCameraPosition;

void main(void)
{
	FragNormal = vec3(MVMatrix * vec4(in_Vertex_Normal, 0.0));
	FragPosition =  vec4(ViewMatrix * ModelMatrix  * in_Position);
	//DirectionLightEye = vec3(ViewMatrix * vec4(DirectionLight, 0.0));
	
	vec4 cameraPosn = vec4(ViewMatrix * vec4(WorldCameraPosition, 1.0));
	vec3 worldView = vec3( cameraPosn - FragPosition );
	ReflectDir = reflect(-worldView, FragNormal );
	
	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * in_Position;
}