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
const float BAND_GAPY   = 0.600000 * 2.0;
const float BAND_STEPX  = BAND_WIDTH + BAND_GAPX;
const float BAND_STEPY  = BAND_LENGTH + BAND_GAPY;

const float ANGLE_SEGMENTS = 16.0;
const float ANGLE_SUB_STEP = 1.0 / ANGLE_SEGMENTS / 3.0 * PI2;

float rand(float a, float b){
    return fract(sin(dot(vec2(a, b), vec2(12.9898,78.233))) * 43758.5453);
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

float getColumn(vec2 pt)
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

float getRain(vec2 pt, float seed, float threshold)
{
    float c = floor(pt.x / BAND_STEPX);
    float v = getColumn(pt + vec2(0, rand(seed, c) * BAND_STEPY));
    return v > threshold ? 1.0 : 0.0;
}

float getRains(vec2 pt, float seed, float threshold)
{
    float rain0 = 1.0 - getRain(rotate2(-ANGLE_SUB_STEP) * pt, seed - 1.0, threshold);
    float rain1 = 1.0 - getRain(pt, seed,       threshold);
    float rain2 = 1.0 - getRain(rotate2(+ANGLE_SUB_STEP) * pt, seed + 1.0, threshold);
    
    return 1.0 - (rain0 * rain1 * rain2);
}

float getThreshold(float angle, float rainAngle)
{
    if (angle < PI / 2.0 || angle > 3.0 * PI / 2.0)
    {
        angle = angle < PI ? angle : angle - 2.0 * PI;
        rainAngle = rainAngle < PI ? rainAngle : rainAngle - 2.0 * PI;
    }
    
    return 1.0 - (cos((angle - rainAngle) * PI * 1.8) / 2.0 + 0.5);
}

float getSketch(vec2 pt, float angle, bool shadow, float strength)
{
    angle = mod(angle, PI2);
    float segment = floor(angle / PI2 * ANGLE_SEGMENTS) + 0.5;
    float segment0 = mod(segment - 1.0, ANGLE_SEGMENTS);
    float segment1 = mod(segment + 0.0, ANGLE_SEGMENTS);
    float segment2 = mod(segment + 1.0, ANGLE_SEGMENTS);
    
    float angle0 = segment0 / ANGLE_SEGMENTS * PI2;
    float angle1 = segment1 / ANGLE_SEGMENTS * PI2;
    float angle2 = segment2 / ANGLE_SEGMENTS * PI2;
    
    if (shadow)
        return 
            (1.0 - getRains(rotate2(angle0) * pt, angle0, getThreshold(angle, angle0))) * 
            (1.0 - getRains(rotate2(angle1) * pt, angle1, getThreshold(angle, angle1))) *
            (1.0 - getRains(rotate2(angle2) * pt, angle2, getThreshold(angle, angle2)));
    else
        return 
            (1.0 - getRain(rotate2(angle0) * pt, angle0, getThreshold(angle, angle0) + (1.0 - strength))) * 
            (1.0 - getRain(rotate2(angle1) * pt, angle1, getThreshold(angle, angle1) + (1.0 - strength))) *
            (1.0 - getRain(rotate2(angle2) * pt, angle2, getThreshold(angle, angle2) + (1.0 - strength)));
}

float getShader(vec2 position, vec3 normal, bool shadow)
{
    float angle = atan(normal.y, normal.x);
    float strength = normal.z > 0.90 ? 0.0 : 
    				 normal.z < 0.75 ? 1.0 : 
    								   1.0 - (normal.z - 0.75) / 0.15;
    
    return getSketch(position, angle, shadow, strength);
}

void main()
{
  // Throw away the fragment if less than the draw percentage
  if ((1.0f - order_vert_out) > draw_percentage)
	discard;

  vec2 point = gl_FragCoord.xy + gl_SamplePosition; // TODO: manual aliasing, using gl_SamplePosition forces this fragment shader to be ran once per sample instead of once per pixel : https://forum.unity.com/threads/anti-aliasing-does-not-work-when-shapes-are-drawn-from-fragment-shader.264771/
  point.x = 2.0f * point.x / 1280.0f - 1;
  point.y = 2.0f * point.y / 720.0f - 1;
  point.y = point.y / ratio;
  float angle = length(point) * radians(40);

  vec2 position = point;
  vec3 normal = rotate3(vec3(point.y, -point.x, 0), -angle) * norml_geom_out;
  //vec3 normal = norml_geom_out;
  bool shadow = false; // TODO: shadows
  float val = getShader(position, normal, shadow);

  gl_FragColor = vec4(val, val, val, 1.0);
}