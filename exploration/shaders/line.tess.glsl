#version 420
 
layout(isolines) in;
in float order_tesc_out[];

out vec4 tess_vertex_offset;
out float order_tess_out;

uniform float frame;

float point_variation(float t)
{
	// Returns a moderately varied value based on t
	// - must be between 0 and 1
	// - must return 0 at t=0 and t=1

	float pi = 3.14159265358979f;
	return sin(t*pi) * (cos(17*t*pi + frame) + cos(13*t*pi + 2*frame)) * 0.5;
}
 
void main()
{
	// The determining value comes in through gl_TessCoord.x; the other values are
	// not used and will be zero.

	float t = gl_TessCoord.x;

	gl_Position = mix(gl_in[1].gl_Position, gl_in[2].gl_Position, t);
	order_tess_out = mix(order_tesc_out[1], order_tesc_out[2], t);
	tess_vertex_offset = vec4(
		point_variation(t+0),
		point_variation(t+1),
		point_variation(t+2),
		point_variation(t+3)
	);
}
