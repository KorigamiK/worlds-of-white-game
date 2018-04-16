#version 330 core

layout(lines_adjacency) in;
layout(triangle_strip, max_vertices = 64) out;

uniform sampler2D depth_texture;
uniform float frame;
uniform float ratio;

// even number of internal points so that end can always be same direction
// 
// start     |       |      end
//  /--+ 3---- + 5---- + 7---- + 8\
// + 1 |       |       |       |   + 10
// |   *       *       *       *   |
// + 0 |       |       |       |   + 11
//  \--+ 2---- + 4---- + 6---- + 9/
// 

const int   POINT_MAX = 28;        // maximum intermediate points
const float POINT_SPACING = 0.05f; // (screen units) length between points 

const float EDGE_OFFSET = 0.004f;          // (screen units) edge distance from point base
const float EDGE_OFFSET_VARIANCE = 0.001; // (screen units) edge distance from point variation

const float POINT_DRIFT_MAX = EDGE_OFFSET - EDGE_OFFSET_VARIANCE; // (screen units) maximum point wobble
//const float POINT_DRIFT_MAX = 0.0f;                               // (screen units) maximum point wobble
const float POINT_DRIFT = POINT_DRIFT_MAX / 2;                    // (screen units) point wobble

float seed1 = frame;
float seed2 = gl_PrimitiveIDIn;
float rand()
{
  float value = fract(sin(dot(vec2(seed1, seed2) ,vec2(12.9898, 78.233))) * 43758.5453) * 2.0f - 1.0f;
  seed2 = seed1;
  seed1 = value;
  return value;
}

float get_depth_1(vec2 v)
{
	// Convert projection space (-1.0 to 1.0) to texture space (0.0 to 1.0)

	float x = v.x / 2.0f + 0.5f;
	float y = v.y / 2.0f + 0.5f;

	float value = texture(depth_texture, vec2(x, y)).x;
	
	// For some reason, the depth texture has values between 0.5 and 1.0. This
	// normalizes it to be between 0.0 and 1.0

	return value * 2.0f - 1.0f;
}

float get_depth(vec2 v)
{
	// Because of reasons, I get the deepest point from nearby texture points

	ivec2 size = textureSize(depth_texture, 0);

	vec2 dx = vec2(1.0f / size.x, 0.0f);
	vec2 dy = vec2(0.0f, 1.0f / size.y);

	float d1 = get_depth_1(v + dx);
	float d2 = get_depth_1(v - dx);
	float d3 = get_depth_1(v + dy);
	float d4 = get_depth_1(v - dy);

	return max(max(d1, d2), max(d3, d4));
}

float percent_difference(float a, float b)
{
	return (a - b) / b;
}

bool is_hidden(vec4 p)
{
	return percent_difference(p.z, get_depth(p.xy)) > (0.00f * (1.0f - p.z));
}

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

    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	float d1 = direction_from_line(p1.xy, p2.xy, p0.xy);
	float d2 = direction_from_line(p1.xy, p2.xy, p3.xy);

	// Check includes zero on the off-chance one face-point is exactly on the 
	// line; it should be considered an edge in that case.

	return d1 * d2 >= 0;
}

void _draw_segment(vec4 p, vec4 perp)
{
	gl_Position = p + perp * (EDGE_OFFSET + rand() * EDGE_OFFSET_VARIANCE);
	EmitVertex();

	gl_Position = p - perp * (EDGE_OFFSET + rand() * EDGE_OFFSET_VARIANCE);
	EmitVertex();
}

void _draw_start_endcap(vec4 p, vec4 perp)
{
	vec4 para = vec4(perp.y, -perp.x, 0.0f, 0.0f);

	gl_Position = p + para * EDGE_OFFSET * 0.66f + perp * EDGE_OFFSET / 2;
	EmitVertex();
	
	gl_Position = p + para * EDGE_OFFSET * 0.66f - perp * EDGE_OFFSET / 2;
	EmitVertex();
}

void _draw_end_endcap(vec4 p, vec4 perp)
{
	vec4 para = vec4(perp.y, -perp.x, 0.0f, 0.0f);

	gl_Position = p - para * EDGE_OFFSET * 0.66f + perp * EDGE_OFFSET / 2;
	EmitVertex();
	
	gl_Position = p - para * EDGE_OFFSET * 0.66f - perp * EDGE_OFFSET / 2;
	EmitVertex();
	
	EndPrimitive();
}

vec4 _find_intermediate(vec4 p_hidden, vec4 p_shown)
{
	vec4 middle;
	for (int i = 0; (distance(p_hidden.xy, p_shown.xy) > EDGE_OFFSET * 2) && (i < 5); ++i)
	{
		vec4 middle = mix(p_hidden, p_shown, 0.5);
		if (is_hidden(middle))
			p_hidden = middle;
		else
			p_shown = middle;
	}
	return p_shown;
}

float drift = rand() * POINT_DRIFT;
vec4 previous_p = vec4(0.0f, 0.0f, 0.0f, 0.0f);
bool previous_p_draw = false;
void draw_segment(vec4 p, bool p_draw, vec4 perp)
{
	drift = clamp(drift + rand() * POINT_DRIFT, -POINT_DRIFT_MAX, POINT_DRIFT_MAX);

	if (previous_p_draw && p_draw)
	{
		_draw_segment(p + perp * drift, perp);
	}

	if (previous_p_draw && !p_draw)
	{
		// find intermediate
		vec4 m = _find_intermediate(p, previous_p) + perp * drift;
		_draw_end_endcap(m, perp);
	}

	if (!previous_p_draw && p_draw)
	{
		// find intermediate
		vec4 m = _find_intermediate(previous_p, p) + perp * drift;
		_draw_start_endcap(m, perp);
		_draw_segment(m, perp);
	}

	if (!previous_p_draw && !p_draw)
	{
		// do nothing
	}

	previous_p = p;
	previous_p_draw = p_draw;
}

void draw_segment_start(vec4 p, bool p_draw, vec4 perp)
{
	if (p_draw)
	{
		_draw_start_endcap(p + perp * drift, perp);
		_draw_segment(p + perp * drift, perp);
	}

	previous_p = p;
	previous_p_draw = p_draw;
}

void draw_segment_end(vec4 p, bool p_draw, vec4 perp)
{
	drift = 0.0f;
	draw_segment(p, p_draw, perp);
	if (previous_p_draw)
	{
		_draw_end_endcap(previous_p, perp);
	}
}

void main()
{
	if (!is_edge())
		return;

    vec4 start = gl_in[1].gl_Position;
	vec4 end = gl_in[2].gl_Position;

	vec4 diff = end - start;
	vec4 direction = normalize(diff);
	vec4 perp = vec4(direction.y, -direction.x / ratio, 0.0f, 0.0f);
	float virtual_length = length(diff);
	float screen_length = length(vec2(diff.x / ratio, diff.y));
	int n = min(POINT_MAX, int(screen_length / POINT_SPACING) - 1);

	draw_segment_start(start, !is_hidden(start), perp);
	
	for (int i = 0; i < n; ++i)
	{
		vec4 p = mix(start, end, (i + 1.0f) / (n + 1.0f));
		draw_segment(p, !is_hidden(p), perp);
	}
	
	draw_segment_end(end, !is_hidden(end), perp);
}
