#version 420 core

layout(triangles) in;
in float order_vert_out[3];
in vec4  world_vert_out[3];

layout(triangle_strip, max_vertices = 3) out;
out vec3  norml_geom_out;
out float order_geom_out;
out vec4  world_geom_out;
out vec3  world_normal_geom_out;

vec3 calculateNormal(vec3 p1, vec3 p2, vec3 p3)
{
	return normalize(cross(p2 - p1, p3 - p1));
}

void main()
{
	vec3 normal = calculateNormal(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);
	vec3 world_normal = calculateNormal(world_vert_out[0].xyz, world_vert_out[1].xyz, world_vert_out[2].xyz);

	gl_Position = gl_in[0].gl_Position;
	norml_geom_out = normal;
	order_geom_out = order_vert_out[0];
	world_geom_out = world_vert_out[0];
	world_normal_geom_out = world_normal;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	norml_geom_out = normal;
	order_geom_out = order_vert_out[1];
	world_geom_out = world_vert_out[1];
	world_normal_geom_out = world_normal;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	norml_geom_out = normal;
	order_geom_out = order_vert_out[2];
	world_geom_out = world_vert_out[2];
	world_normal_geom_out = world_normal;
	EmitVertex();
}
