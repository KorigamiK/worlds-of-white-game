#version 420 core
 
layout(vertices = 4) out;

uniform float frame;
uniform float ratio;

const float POINT_SPACING = 2.0f / 64.0f;

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

vec2 get_screen_space_vector()
{
	// This determines the vector of the line projected on the screen for 
	// determining its visual length

    vec4 start = gl_in[1].gl_Position / gl_in[1].gl_Position.w;
	vec4 end   = gl_in[2].gl_Position / gl_in[2].gl_Position.w;

	float dx = end.x - start.x;
	float dy = end.y - start.y;
	
	return vec2(dx * ratio, dy);
}

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	if (gl_InvocationID == 0)
	{
		// Zero here means don't draw the line (create no segments). This
		// shader is in control of which lines to draw; the geometry shader
		// is responsible for how to draw them (depth, effects, etc)

		int segment_count = 0;

		if (is_edge())
		{
			// Implementations may allow for more than 64 segments, but it is
			// the spec-defined minimum, so I'll just use that for now.

			float line_length = length(get_screen_space_vector());
			segment_count = clamp(int(line_length / POINT_SPACING + 0.5f), 1, 64);
		}

		gl_TessLevelOuter[0] = 1;
		gl_TessLevelOuter[1] = segment_count;
	}
}
