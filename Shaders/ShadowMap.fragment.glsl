#version 400

//out float fragmentdepth;
out vec4 out_Color;

void main(void)
{
	/*fragmentdepth =  gl_FragCoord.z;
	out_Color.x = gl_FragCoord.z;
    out_Color.rgb=vec3(pow(gl_FragCoord.z,50)); */
	out_Color.rgb=vec4(gl_FragCoord.z);
}   