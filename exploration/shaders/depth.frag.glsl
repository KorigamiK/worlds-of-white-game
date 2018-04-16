#version 330 core

void main()
{
  // This just returns black for all values, however these pixels won't be 
  // used. This shader is only here for the depth buffer
  gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0);

}