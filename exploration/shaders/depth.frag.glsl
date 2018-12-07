#version 420 core

// in vec2 uv_coords;

uniform sampler2D model_texture;

void main()
{
  // This just returns black for all values, however these pixels won't be 
  // used. This shader is only here for the depth buffer
  //vec4 c = texture(model_texture, vec2(uv_coords.x, 1.0 - uv_coords.y));
  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  //gl_FragColor = c;

}