#version 420 core

layout(triangles) in;
in float order_vert_out[3];
in vec4  rfpos_vert_out[3];

layout(triangle_strip, max_vertices = 3) out;
out vec3  norml_geom_out;
out float order_geom_out;
out vec4  rfpos_geom_out;

vec3 calculateNormal(vec3 p1, vec3 p2, vec3 p3)
{
	return normalize(cross(p2 - p1, p3 - p1));
}

void main()
{
	vec3 normal = calculateNormal(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);

	gl_Position = gl_in[0].gl_Position;
	norml_geom_out = normal;
	order_geom_out = order_vert_out[0];
	rfpos_geom_out = rfpos_vert_out[0];
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	norml_geom_out = normal;
	order_geom_out = order_vert_out[1];
	rfpos_geom_out = rfpos_vert_out[1];
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	norml_geom_out = normal;
	order_geom_out = order_vert_out[2];
	rfpos_geom_out = rfpos_vert_out[2];
	EmitVertex();
}
