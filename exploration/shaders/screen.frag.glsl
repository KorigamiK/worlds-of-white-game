#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D face_texture;
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

void main()
{
    vec3 face = texture(face_texture, TexCoords).rgb;
    vec3 line = textureMultisample(line_texture, TexCoords).rgb;
    vec3 bkgd = texture(bkgd_texture, TexCoords).rgb;
    vec3 debg = texture(debg_texture, TexCoords).rgb;

	if (face.x + face.y + face.z != 3.0)
		face = 1.0 - (1.0 - face) * 0.75f;

	vec3 final = bkgd * line * debg;

    FragColor = vec4(final.x, final.y, final.z, 1.0);
}
