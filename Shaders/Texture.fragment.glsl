#version 400

in vec3 FragNormal;
in vec4 FragPosition;
in vec3 DirectionLightEye;
in vec3 TexPosn;
in vec4 inColour;
out vec4 out_Color;

uniform vec4 DirectionLightColour;
uniform vec3 ViewVector;
uniform sampler2D gaussianTexture;
uniform sampler2D bumpTexture;

void main(void)
{
	vec4 specColor = vec4(1.0, 1.0,1.0,1.0);
	float shininess = 38.2;
	vec4 bumpNormal = texture(bumpTexture, vec2(TexPosn));
	vec3 newNormal = vec3(bumpNormal) + FragNormal;
	vec3 n = normalize(newNormal);
	vec3 s = normalize(DirectionLightEye - vec3(FragPosition));
	vec3 r = normalize(reflect(-s, n));
	vec3 v = normalize(vec3(-FragPosition));
	
	vec4 ambcolor = max( dot(normalize(DirectionLightEye), n), 0.0 ) * DirectionLightColour ;
	vec4 ambLight = vec4(0, 0.1, 0, 0);
	ambcolor = ambcolor + inColour;
	
	vec4 specHighlight = pow( max( dot(r,v), 0.0 ), shininess ) * specColor;
	
	vec2 texCoord = normalize(FragPosition.xy);
	vec4 guassianTerm = texture(gaussianTexture, vec2(TexPosn));
	
	vec3 col = mix(ambcolor.rgb, guassianTerm.rgb, guassianTerm.a);
	out_Color =  vec4(col, ambcolor.a) + specHighlight + ambLight;
}