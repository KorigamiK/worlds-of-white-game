#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D face_texture;
uniform sampler2D depth_texture;
uniform sampler2DMS line_texture;
uniform sampler2D bkgd_texture;
uniform sampler2D debg_texture;
uniform int line_texture_samples;

vec4 textureMultisample(sampler2DMS sampler, vec2 uv)
{
	ivec2 ssize = textureSize(sampler);
	ivec2 coord = ivec2(uv * ssize);

    vec4 color = vec4(0.0);
    for (int i = 0; i < line_texture_samples; i++)
        color += texelFetch(sampler, coord, i);
    color /= float(line_texture_samples);

    return color;
}

float LinearizeDepth(float depth)
{
    float zNear = 0.1;
    float zFar  = 100.0;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
    vec3 face = texture(face_texture, TexCoords).rgb;
    vec3 dpth = texture(depth_texture, TexCoords).rgb;
    vec3 line = textureMultisample(line_texture, TexCoords).rgb;
    vec3 bkgd = texture(bkgd_texture, TexCoords).rgb;
    vec3 debg = texture(debg_texture, TexCoords).rgb;

	// DEBUG DEPTH + NORMALS
	//if (face.x + face.y + face.z != 3.0)
	//{
	//	float d = LinearizeDepth(dpth.x);
	//	face.x = face.x;
	//	face.y = face.y;
	//	face.z = d;
	//}

	// SHADE STRENGTH
	if (face.x + face.y + face.z != 3.0)
	{
		float d = face.z * 2 - 1;
		d = (1.0f - (1.0f - d) / 4.0f);
		face.x = d;
		face.y = d;
		face.z = d;
	}

	// vec3 final = bkgd * line * debg;
	vec3 final = bkgd * line * debg * face;

    FragColor = vec4(final.x, final.y, final.z, 1.0);
}
