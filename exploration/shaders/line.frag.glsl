#version 330 core

out vec4 FragColor;

void main()
{
    // set black
	FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// set depth color
	// float c = LinearizeDepth(gl_FragCoord.z);
	// FragColor = vec4(c, c, c, 1.0f);
}
