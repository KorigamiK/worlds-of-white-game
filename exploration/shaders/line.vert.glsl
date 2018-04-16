#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float ratio;
uniform float frame;

const float MAX_VERTEX_OFFSET = 0.01f; // maximum vertex offset (screen units)

float seed1 = frame;
float seed2 = gl_VertexID;
float rand()
{
  float value = fract(sin(dot(vec2(seed1, seed2) ,vec2(12.9898, 78.233))) * 43758.5453) * 2.0f - 1.0f;
  seed2 = seed1;
  seed1 = value;
  return value;
}

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	gl_Position /= gl_Position.w;

	gl_Position.x += rand() * MAX_VERTEX_OFFSET;
	gl_Position.y += rand() * MAX_VERTEX_OFFSET;
}
