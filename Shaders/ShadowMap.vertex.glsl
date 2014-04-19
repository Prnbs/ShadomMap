#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;

uniform mat4 ShadowModelMatrix;
uniform mat4 ShadowViewMatrix;
uniform mat4 OrthoProjectionMatrix;
out vec4 inColour;
void main(void)
{
	inColour = in_Color;
	gl_Position = (OrthoProjectionMatrix * ShadowViewMatrix * ShadowModelMatrix) * in_Position;
} 
