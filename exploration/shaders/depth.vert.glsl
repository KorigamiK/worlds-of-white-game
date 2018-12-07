#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aGroups;
layout (location = 2) in vec3 aWeights;
// layout (location = 3) in vec2 uv_coords_in;

// out vec2 uv_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 view_reference;

uniform float ratio;
uniform float frame;

uniform mat4 positions[24];

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
	vec4 model_space_position = model * vec4(aPos, 1.0f);
	model_space_position /= model_space_position.w;
	model_space_position.xyz = apply_variation(model_space_position.xyz);
	
	vec4 pos0 = projection * view * model * positions[int(aGroups.x)] * vec4(aPos, 1.0f);
	vec4 pos1 = projection * view * model * positions[int(aGroups.y)] * vec4(aPos, 1.0f);
	vec4 pos2 = projection * view * model * positions[int(aGroups.z)] * vec4(aPos, 1.0f);
	
	gl_Position = (aWeights[0] * pos0) + (aWeights[1] * pos1) + (aWeights[2] * pos2);
	// uv_coords = uv_coords_in;
}
