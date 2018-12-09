#version 420 core

in float order_vert_out;

uniform float draw_percentage;

void main()
{
  // Throw away the fragment if less than the draw percentage
  if ((1.0f - order_vert_out) > draw_percentage)
	discard;

  // This just returns black for all values, however these pixels won't be 
  // used. This shader is only here for the depth buffer
  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}