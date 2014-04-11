#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;

uniform mat4 ShadowModelMatrix;
uniform mat4 ShadowViewMatrix;
uniform mat4 OrthoProjectionMatrix;

void main(void)
{
	gl_Position = (OrthoProjectionMatrix * ShadowViewMatrix * ShadowModelMatrix) * in_Position;
} 
