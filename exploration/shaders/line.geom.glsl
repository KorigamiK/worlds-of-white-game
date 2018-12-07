#version 420 core

layout(lines) in;
layout(triangle_strip, max_vertices = 8) out;

in vec4 tess_vertex_offset[2];

uniform sampler2D depth_texture;

uniform float frame;
uniform float ratio;

// Keeping the ratio correct is a pain, so I just convert to screen space, do
// any manipulations, then put it back into the projection space. This might be 
// supplied later through a unifrom variable.

const mat4 to_screen_space = mat4(
	1, 0,     0, 0,
	0, 1/ratio, 0, 0,
	0, 0,     1, 0,
	0, 0,     0, 1
);
const mat4 from_screen_space = inverse(to_screen_space);

// Always has this shape:
// 
//   start                    end
//  /--+ 3-------------------- + 8\
// + 1 |                       |   + 10
// |   *                       *   |
// + 0 |                       |   + 11
//  \--+ 2-------------------- + 9/
// 

const float EDGE_OFFSET = 0.002f;          // (screen units) edge distance from point base
//const float EDGE_OFFSET = 0.002f;          // (screen units) edge distance from point base
//const float EDGE_OFFSET_VARIANCE = 0.0015; // (screen units) edge distance from point variation
const float EDGE_OFFSET_VARIANCE = 0.000f; // (screen units) edge distance from point variation

float seed1 = frame;
float seed2 = gl_PrimitiveIDIn; // this is a bad seed value, its not unique after tessellation
float rand()
{
  float value = fract(sin(dot(vec2(seed1, seed2) ,vec2(12.9898, 78.233))) * 43758.5453) * 2.0f - 1.0f;
  seed2 = seed1;
  seed1 = value;
  return value;
}

mat4 rotate(float theta)
{
	return mat4(
		cos(theta), -sin(theta), 0, 0,
		sin(theta),  cos(theta), 0, 0,
		0,           0,          1, 0,
		0,           0,          0, 1
	);
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

bool is_hidden(vec4 p)
{
	return p.z / p.w > get_depth(p.xy / p.w);
}

void _draw_segment(vec4 p, vec4 perp, float thickness)
{
	gl_Position = p + perp * (EDGE_OFFSET + thickness * EDGE_OFFSET_VARIANCE) * p.w;
	gl_Position = from_screen_space * gl_Position;
	EmitVertex();

	gl_Position = p - perp * (EDGE_OFFSET + thickness * EDGE_OFFSET_VARIANCE) * p.w;
	gl_Position = from_screen_space * gl_Position;
	EmitVertex();
}

void _draw_start_endcap(vec4 p, vec4 perp)
{
	vec4 para = vec4(perp.y, -perp.x, 0.0f, 0.0f);

	gl_Position = p + para * EDGE_OFFSET * 0.66f * p.w + perp * EDGE_OFFSET / 2 * p.w;
	gl_Position = from_screen_space * gl_Position;
	EmitVertex();
	
	gl_Position = p + para * EDGE_OFFSET * 0.66f * p.w - perp * EDGE_OFFSET / 2 * p.w;
	gl_Position = from_screen_space * gl_Position;
	EmitVertex();
}

void _draw_end_endcap(vec4 p, vec4 perp)
{
	vec4 para = vec4(perp.y, -perp.x, 0.0f, 0.0f);

	gl_Position = p - para * EDGE_OFFSET * 0.66f * p.w + perp * EDGE_OFFSET / 2 * p.w;
	gl_Position = from_screen_space * gl_Position;
	EmitVertex();
	
	gl_Position = p - para * EDGE_OFFSET * 0.66f * p.w - perp * EDGE_OFFSET / 2 * p.w;
	gl_Position = from_screen_space * gl_Position;
	EmitVertex();
}

vec4 _find_intermediate(vec4 p_hidden, vec4 p_shown)
{
	vec4 middle;
	for (int i = 0; (distance(p_hidden.xy / p_hidden.w, p_shown.xy / p_shown.w) > EDGE_OFFSET * 2) && (i < 5); ++i)
	{
		vec4 middle = mix(p_hidden, p_shown, 0.5);
		if (is_hidden(middle))
			p_hidden = middle;
		else
			p_shown = middle;
	}
	return p_shown;
}

float transform1(float v)
{
	// v between -1 and 1

	return 1 - cos(v*3.141592/2);
}

void draw_segment(vec4 p1, vec4 p2)
{
	p1 = to_screen_space * p1;
	p2 = to_screen_space * p2;

	vec4 perp = normalize(vec4(p1.y / p1.w - p2.y / p2.w, p2.x / p2.w - p1.x / p1.w, 0.0f, 0.0f));
	
	//// SKETCHY
	//perp = perp * rotate(0.0 + 0.5f * tess_vertex_offset[0].y);
	//vec4 para = vec4(perp.y, -perp.x, 0.0f, 0.0f);
	//vec4 center = mix(p1, p2, 0.5);
	//float l = distance(p1.xy / p1.w, p2.xy / p2.w) * 3.0;
	//p1 = center + para * l / 2 * center.w;
	//p2 = center - para * l / 2 * center.w;

	//// WAVE
	//vec4 center = mix(p1, p2, 0.5);
	//float angle = atan(p1.y - p2.y, p1.x - p2.x);
	//perp = vec4(0, 1, 0, 0);
	//vec4 para = vec4(perp.y, -perp.x, 0.0f, 0.0f);
	//float l = distance(p1.xy / p1.w, p2.xy / p2.w) * 3.0;
	//p1 = center + para * l / 2 * center.w;
	//p2 = center - para * l / 2 * center.w;

	//// SPIKE
	//vec2 center = vec2(0.33, 0.33);
	//p1 += vec4(normalize(p1.xy / p1.w - center), 0, 0) * transform1(tess_vertex_offset[0].y) * 0.2f * p1.w;
	//p2 += vec4(normalize(p2.xy / p2.w - center), 0, 0) * transform1(tess_vertex_offset[1].y) * 0.2f * p2.w;

	perp = normalize(vec4(p1.y / p1.w - p2.y / p2.w, p2.x / p2.w - p1.x / p1.w, 0.0f, 0.0f));

	_draw_start_endcap(p1, perp);
	_draw_segment(p1, perp, tess_vertex_offset[0].x);
	_draw_segment(p2, perp, tess_vertex_offset[1].x);
	_draw_end_endcap(p2, perp);
	
	EndPrimitive();
}

void main()
{
    vec4 start = gl_in[0].gl_Position;
	vec4 end = gl_in[1].gl_Position;

	bool start_shown = !is_hidden(start);
	bool end_shown   = !is_hidden(end);

	if (start_shown && end_shown)
	{
		draw_segment(start, end);
	}
	else if (start_shown && !end_shown)
	{
		vec4 new_end = _find_intermediate(end, start);
		draw_segment(start, new_end);
	}
	else if (!start_shown && end_shown)
	{
		vec4 new_start = _find_intermediate(start, end);
		draw_segment(new_start, end);
	}
	else if (start_shown && end_shown)
	{
		// don't draw anything
	}
}
