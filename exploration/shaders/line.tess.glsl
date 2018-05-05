#version 420
 
layout(isolines) in;
 
void main()
{
	float t = gl_TessCoord.x;
	gl_Position = mix(gl_in[1].gl_Position, gl_in[2].gl_Position, t) + sin(t * 3.1415926 * 2) * vec4(0.01, 0.01, 0.01, 0.0);
}
