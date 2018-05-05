#version 420 core
 
layout(vertices = 4) out; // 4 points per patch

float direction_from_line(vec2 p1, vec2 p2, vec2 point)
{
	// The standard equation for distance from point to line omitting the abs()
	// and the sqrt(). The reason being that which side of the line the point 
	// lies is the end goal, not the actual distance.
	// 
	// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line

	return (p2.y - p1.y) * point.x - (p2.x - p1.x) * point.y + p2.x * p1.y - p2.y * p1.x;
}

bool is_edge()
{
	// The line is an edge if both face-points lie on the same side of the line.

    vec4 p0 = gl_in[0].gl_Position / gl_in[0].gl_Position.w;
    vec4 p1 = gl_in[1].gl_Position / gl_in[1].gl_Position.w;
	vec4 p2 = gl_in[2].gl_Position / gl_in[2].gl_Position.w;
	vec4 p3 = gl_in[3].gl_Position / gl_in[3].gl_Position.w;

	float d1 = direction_from_line(p1.xy, p2.xy, p0.xy);
	float d2 = direction_from_line(p1.xy, p2.xy, p3.xy);

	// Check includes zero on the off-chance one face-point is exactly on the 
	// line; it should be considered an edge in that case.

	return d1 * d2 >= -0.000000000001f;
}

void main() {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	if (gl_InvocationID == 0)
	{ // levels only need to be set once per patch
		
		int n = 5;
		if (!is_edge())
			n = 0;

		gl_TessLevelOuter[0] = 1; // we're only tessellating one line
		gl_TessLevelOuter[1] = n; // tessellate the line into 100 segments
	}
}
