#version 330 core
out vec4 FragColor;

float LinearizeDepth(float depth)
{
    float zNear = 0.1;
    float zFar  = 100.0;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
    // set black
	FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// set depth color
	// float c = LinearizeDepth(gl_FragCoord.z);
	// FragColor = vec4(c, c, c, 1.0f);
}
