#version 420 core

in vec3 norml_geom_out;
in float order_vert_out;

uniform float ratio;
uniform float draw_percentage;

const float PI = 3.14159265358979;
const float PI2 = 2.0 * PI;

const float BAND_WIDTH  = 0.003333;
const float BAND_LENGTH = 0.148148 * 2.0;
const float BAND_GAPX   = 0.004000;
const float BAND_GAPY   = 0.400000;
const float BAND_STEPX  = BAND_WIDTH + BAND_GAPX;
const float BAND_STEPY  = BAND_LENGTH + BAND_GAPY;

const float ANGLE_SEGMENTS = 16.0;
const float ANGLE_SUB_STEP = 1.0 / ANGLE_SEGMENTS / 3.0 * PI2;

float rand(float a, float b)
{
    return fract(sin(dot(vec2(a, b), vec2(12.9898,78.233))) * 43758.5453);
}

float rand(vec3 pos)
{
    return fract(sin(dot(pos, vec3(12.9898,78.233,183.1415))) * 43758.5453);
}

float smoothr(float p1, float p2, float d)
{
	// This is a smoothstep() implementation that works more like mix() in
	// regards to the interpolation value.

    return mix(p1, p2, d * d * (3.0 - 2.0 * d));
}

float perlin(vec3 pos)
{
	// This is a 3D perlin noise function. The smoothr() function above is used
	// to smooth the output between control point transitions.

    vec3 p1 = floor(pos);
    vec3 p2 = p1 + vec3(1.0);
    vec3 p3 = vec3(p1.x, p2.y, p1.z);
    vec3 p4 = vec3(p2.x, p1.y, p1.z);
    vec3 p5 = vec3(p1.x, p1.y, p2.z);
    vec3 p6 = vec3(p2.x, p1.y, p2.z);
    vec3 p7 = vec3(p1.x, p2.y, p2.z);
    vec3 p8 = vec3(p2.x, p2.y, p1.z);
    vec3 dv = pos - p1;
    
    return
        smoothr(
            smoothr(
                smoothr(rand(p1), rand(p5), dv.z), 
                smoothr(rand(p3), rand(p7), dv.z), 
                dv.y),
            smoothr(
                smoothr(rand(p4), rand(p6), dv.z), 
                smoothr(rand(p8), rand(p2), dv.z), 
                dv.y),
            dv.x
        );
}

float perlinMixer(vec3 pos)
{
	// The noise function, perlin(), naturally has cyclic properties that we'd
	// like to avoid. This mixes two perlin() noise values at a half-step off
	// from eachother to level out the cyclic patterns. More noise values could
	// be used to generate a smoother result but would also produce a result
	// with less overall variation, so this is a balanced result.

    float val = mix(
        perlin(+pos),
        perlin(-pos + 0.5),
        0.5);
    
    return smoothstep(0.25, 0.75, val);
}

mat2 rotate2(float angle) {
	return mat2(
        cos(angle), -sin(angle), 
        sin(angle),  cos(angle)
    );
}

mat3 rotate3(vec3 axis, float angle)
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

float getLine(vec2 pt)
{
   	const float BAND_STEPX_1 = BAND_GAPX / 2.0;
    const float BAND_STEPX_2 = BAND_STEPX - BAND_GAPX / 2.0;
    const float BAND_STEPY_1 = BAND_GAPY / 2.0 - BAND_WIDTH / 2.0;
    const float BAND_STEPY_2 = BAND_GAPY / 2.0;
    const float BAND_STEPY_3 = BAND_STEPY - BAND_GAPY / 2.0;
    const float BAND_STEPY_4 = BAND_STEPY - BAND_GAPY / 2.0 + BAND_WIDTH / 2.0;
    const vec2 DOT1 = vec2(BAND_STEPX / 2.0, BAND_GAPY / 2.0);
    const vec2 DOT2 = vec2(BAND_STEPX / 2.0, BAND_STEPY - BAND_GAPY / 2.0);

    vec2 f = mod(pt, vec2(BAND_STEPX, BAND_STEPY));
    
    if (f.x < BAND_STEPX_1 || f.x > BAND_STEPX_2)
        return 0.0;
    
    if (f.y < BAND_STEPY_1 || f.y > BAND_STEPY_4)
        return 0.0;
    
    if (f.y >= BAND_STEPY_2 && f.y <= BAND_STEPY_3)
        return 1.0 - abs(BAND_STEPX / 2.0 - f.x) / (BAND_WIDTH / 2.0);
    
    if (f.y < BAND_STEPY_2)
        return 1.0 - distance(DOT1, f) / (BAND_WIDTH / 2.0);
    else
        return 1.0 - distance(DOT2, f) / (BAND_WIDTH / 2.0);
}

float getLines(vec2 pt, float seed, float threshold)
{
	float column = floor(pt.x / BAND_STEPX);
	float x_variation = 0.0;
    float y_variation = rand(seed, column) * BAND_STEPY;
    float t_variation = rand(seed, column) / 2.0;

    float v = getLine(pt + vec2(x_variation, y_variation));

    return v > (threshold + t_variation) ? 1.0 : 0.0;
}

float getAngle(vec3 normal)
{
      vec3 vertices[12];
      vertices[0] = normalize(vec3(-0.26286500f, 0.0000000f, 0.42532500f));
      vertices[1] = normalize(vec3(0.26286500f, 0.0000000f, 0.42532500f));
      vertices[2] = normalize(vec3(-0.26286500f, 0.0000000f, -0.42532500f));
      vertices[3] = normalize(vec3(0.26286500f, 0.0000000f, -0.42532500f));
      vertices[4] = normalize(vec3(0.0000000f, 0.42532500f, 0.26286500f));
      vertices[5] = normalize(vec3(0.0000000f, 0.42532500f, -0.26286500f));
      vertices[6] = normalize(vec3(0.0000000f, -0.42532500f, 0.26286500f));
      vertices[7] = normalize(vec3(0.0000000f, -0.42532500f, -0.26286500f));
      vertices[8] = normalize(vec3(0.42532500f, 0.26286500f, 0.0000000f));
      vertices[9] = normalize(vec3(-0.42532500f, 0.26286500f, 0.0000000f));
      vertices[10] = normalize(vec3(0.42532500f, -0.26286500f, 0.0000000f));
      vertices[11] = normalize(vec3(-0.42532500f, -0.26286500f, 0.0000000f));

      float angles[12];
      angles[0] = 2.56;
      angles[1] = 5.11;
      angles[2] = 0.33;
      angles[3] = 3.87;
      angles[4] = 6.03;
      angles[5] = 2.18;
      angles[6] = 4.62;
      angles[7] = 5.96;
      angles[8] = 1.72;
      angles[9] = 0.97;
      angles[10] = 2.12;
      angles[11] = 0.00;

      int closestIndex = -1;
      float closestAngle = 100.0;
      for (int i = 0; i < 12; ++i)
      {
            float angle = acos(dot(normal, vertices[i]));
            if (angle < closestAngle)
            {
                  closestAngle = angle;
                  closestIndex = i;
            }
      }

      return angles[closestIndex];
}

float getShader(vec3 position, vec3 normal, bool shadow)
{
    float strength = perlinMixer(position * 3.0);
	float angle = getAngle(normal);
    
    return 1.0 - getLines(rotate2(angle) * position.xy, 0.0, strength);
}

float LinearizeDepth(float depth)
{
    float zNear = 0.1;
    float zFar  = 100.0;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
  // Throw away the fragment if less than the draw percentage
  if ((1.0f - order_vert_out) > draw_percentage)
	discard;

  vec3 point = gl_FragCoord.xyz + vec3(gl_SamplePosition, 0.0); // TODO: manual aliasing, using gl_SamplePosition forces this fragment shader to be ran once per sample instead of once per pixel : https://forum.unity.com/threads/anti-aliasing-does-not-work-when-shapes-are-drawn-from-fragment-shader.264771/
  point.x = 2.0f * point.x / 1280.0f - 1;
  point.y = 2.0f * point.y / 720.0f - 1;
  point.y = point.y / ratio;
  point.z = LinearizeDepth(point.z);
  float angle = length(point) * radians(40);

  vec3 position = point;
  vec3 normal = norml_geom_out;
  bool shadow = false; // TODO: shadows
  float val = getShader(position, normal, shadow);

  gl_FragColor = vec4(val, val, val, 1.0);
}