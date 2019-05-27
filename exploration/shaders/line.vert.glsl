#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aGroups;
layout (location = 2) in vec3 aWeights;
layout (location = 3) in float order;

out float order_vert_out;
out float randm_vert_out;
out vec4  world_vert_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 view_reference;

uniform float ratio;
uniform float frame;
uniform float draw_percentage;
uniform mat4 positions[24];

float seed1 = frame;
float seed2 = gl_VertexID; // this is a bad seed value, its not unique between instances
float rand()
{
  float value = fract(sin(dot(vec2(seed1, seed2) ,vec2(12.9898, 78.233))) * 43758.5453) * 2.0f - 1.0f;
  seed2 = seed1;
  seed1 = value;
  return value;
}

float variation(float v)
{
	// This function is used to apply variation with a few guarantees:
	// - Must be deterministic with respect to v
	// - The plot y=x+variation(x) should be monotonically increasing to ensure
	//   overlap cannot occur
	// - The function repeats from -PI to PI
	// - Gives sufficienly uniform results

	float VARIANCE1 = 11;
	float VARIANCE2 = 23;

	// high variance gives more randomness but less overall shifting as the max
	// variance is 1/VARIANCE1 + 1/VARIANCE2

	float v1 = sin(VARIANCE1 * v) / VARIANCE1;
	float v2 = cos(VARIANCE2 * v) / VARIANCE2;

	return (v1 + v2) / 2 * 0.05f;
}

vec3 apply_variation(vec3 point)
{
	// This function modifies a point with respect to the view and frame:
	// - different points with the same direction from the view reference will
	//   have identical variation.
	// - different points with similar direction will have similar variation
	//
	// point should be the world-space position

	vec3 direction = point - view_reference;
	float theta_x = atan(direction.z, direction.x);
	float theta_y = atan(direction.y, length(direction.xz));
	float dist = length(direction);

	float theta_x_2 = theta_x + variation(theta_x + 7 * theta_y + 500 * frame);
	float theta_y_2 = theta_y + variation(theta_y - 7 * theta_x + 500 * frame);
	// dist += variation(theta_x + theta_y + dist + frame);

	float x = dist * cos(theta_x_2) * cos(theta_y_2);
	float z = dist * sin(theta_x_2) * cos(theta_y_2);
	float y = dist * sin(theta_y_2);

	return view_reference + vec3(x, y, z);
}

void main()
{
	vec4 pos0 = model * positions[int(aGroups[0])] * vec4(aPos, 1.0f);
	vec4 pos1 = model * positions[int(aGroups[1])] * vec4(aPos, 1.0f);
	vec4 pos2 = model * positions[int(aGroups[2])] * vec4(aPos, 1.0f);
	vec4 pos = (aWeights[0] * pos0) + (aWeights[1] * pos1) + (aWeights[2] * pos2);
	
	// pos.xyz = apply_variation(pos.xyz); // TODO: this doesn't work
	gl_Position = projection * view * pos;
	order_vert_out = order;
	randm_vert_out = rand();
	world_vert_out = vec4(aPos, 1.0);
}
