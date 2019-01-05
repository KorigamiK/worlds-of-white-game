#version 420 core

in vec3 norml_geom_out;
in float order_vert_out;

uniform float ratio;
uniform float draw_percentage;

mat3 rotate(vec3 axis, float angle)
{
	axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(
		oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
        oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
        oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c
	);
}

void main()
{
  // Throw away the fragment if less than the draw percentage
  if ((1.0f - order_vert_out) > draw_percentage)
	discard;

  vec2 point = gl_FragCoord.xy;
  point.x = 2.0f * point.x / 1280.0f - 1;
  point.y = 2.0f * point.y / 720.0f - 1;
  point.y = point.y / ratio;
  float angle = length(point) * radians(40);

  vec3 normal = rotate(vec3(point.y, -point.x, 0), -angle) * norml_geom_out;

  gl_FragColor = vec4(normal / 2.0f + 0.5f, 1.0);
}