#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D face_texture;
uniform sampler2D line_texture;
uniform sampler2D bkgd_texture;

void main()
{
    vec3 face = texture(face_texture, TexCoords).rgb;
    vec3 line = texture(line_texture, TexCoords).rgb;
    vec3 bkgd = texture(bkgd_texture, TexCoords).rgb;

	if (face.x + face.y + face.z != 3.0)
		face = 1.0 - (1.0 - face) * 0.75f;

	vec3 final = face * bkgd * line;
	//vec3 final = bkgd * line;

    FragColor = vec4(final.x, final.y, final.z, 1.0);
}
