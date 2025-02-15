#version 420 core

in vec3  norml_geom_out;
in float order_geom_out;
in vec4  world_geom_out;
in vec3  world_normal_geom_out;

uniform float frame;
uniform float ratio;
uniform float draw_percentage;
uniform float viewport_width;
uniform float viewport_height;

uniform vec3 base_camera_direction;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 reference_view;
uniform mat4 reference_projection;
uniform int code;

const float PI = 3.14159265358979;
const float PI2 = 2.0 * PI;

const float LINE_LENGTH_RATIO = 20.0; // line is N x the period long
const float LINE_GAP_RATIO    = 0.50; // gap is N x the length
const float LINE_WIDTH_RATIO  = 0.15; // the width relative to the period
const float LINE_WIDTH_RATIO_MIN= 0.015;
const float LINE_SCALE        = 2.0;  // scaling factor
const float ZOOM_SCALE        = 8.0; // period of strength noise vs period of lines
const float WOBBLE_LENGTH_RATIO = 0.25; // ratio of wobble length to line length
const float WOBBLE_WIDTH_RATIO  = 0.125; // ratio of wobble width to line period

float rand(vec3 pos)
{
    pos += 0.01;
    return fract(sin(dot(pos, vec3(12.9898,78.233,183.1415))) * 43758.5453);
}
float rand(vec2 pos)
{
    return fract(sin(dot(pos, vec2(12.9898,78.233))) * 43758.5453);
}
float rand(float pos)
{
    return fract(sin(pos * 12.9898) * 43758.5453);
}

float randInRange(float seed, float minimum, float maximum)
{
    return rand(seed) * (maximum - minimum) + minimum;
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

float smoothr(float p1, float p2, float d)
{
    return mix(p1, p2, d * d * (3.0 - 2.0 * d));
    //return mix(p1, p2, d);
}

float perlin(vec3 pos, float scale)
{
    vec3 p1 = floor(pos / scale) * scale;
    vec3 p2 = floor(p1 / scale + 1.0001) * scale;
    vec3 p3 = vec3(p1.x, p2.y, p1.z);
    vec3 p4 = vec3(p2.x, p1.y, p1.z);
    vec3 p5 = vec3(p1.x, p1.y, p2.z);
    vec3 p6 = vec3(p2.x, p1.y, p2.z);
    vec3 p7 = vec3(p1.x, p2.y, p2.z);
    vec3 p8 = vec3(p2.x, p2.y, p1.z);
    vec3 dv = (pos - p1) / scale;
    
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

float normalizeZoom(float zoom)
{
    //const float ZOOM_RATIO = 0.80;
    //return clamp(zoom * ZOOM_RATIO, 0.0, 8.0);
    
    //return clamp(log(zoom / 5.0 + 1.0) * 5.5, 0.0, 8.0);
 
 	//return clamp(zoom, 0.0, 8.0);
    
    return clamp(-pow(zoom - 10.0, 2.0) / 16.0 + 6.0, 0.0, 8.0);
}

float perlinMixer2(vec3 pos, float zoom)
{
    return smoothstep(0.30, 0.70, 
               mix(
        	       perlin(+pos, zoom * ZOOM_SCALE),
        	       perlin(-pos + zoom * 0.5 * ZOOM_SCALE, zoom * ZOOM_SCALE),
        	       0.5));
}

float perlinMixer(vec3 pos, float zoom)
{
    //zoom = normalizeZoom(zoom);
    
    float zoom1 = floor(-zoom);
    float zoom2 = zoom1 + 1.0;
    float zoomd = (-zoom) - zoom1;
    
    zoom1 = pow(2.0, zoom1);
    zoom2 = pow(2.0, zoom2);
    
    float val = mix(
        mix(
        	perlin(+pos, zoom1 * ZOOM_SCALE),
        	perlin(-pos + zoom1 * 0.5 * ZOOM_SCALE, zoom1 * ZOOM_SCALE),
        	0.5),
        mix(
        	perlin(+pos, zoom2 * ZOOM_SCALE),
        	perlin(-pos + zoom2 * 0.5 * ZOOM_SCALE, zoom2 * ZOOM_SCALE),
        	0.5),
        zoomd);
    
    return smoothstep(0.30, 0.70, val);
    //return val;
}

float drawLine(vec2 pos, vec4 box)
{
    // box:
    //   x = center x
    //   y = center y
    //   z = width
    //   w = height
    
    if (pos.x < box.x - box.z / 2.0 || pos.x > box.x + box.z / 2.0)
        return 1.0; // don't draw
    if (pos.y < box.y - box.w / 2.0 || pos.y > box.y + box.w / 2.0)
        return 1.0; // don't draw
    
    pos = pos - box.xy;
    
    float halfw = box.z / 2.0;
    vec2 p1 = vec2(0.0, box.w / 2.0 - halfw);
    vec2 p2 = vec2(0.0, halfw - box.w / 2.0);
    
    if (pos.y > p1.y)
        return min(distance(pos, p1) / halfw, 1.0);
    if (pos.y < p2.y)
        return min(distance(pos, p2) / halfw, 1.0);
    
    return abs(pos.x) / halfw;
}

float getVal(vec3 pos, float period, float size)
{
    if (size > period / 2.0)
        return 1.0;
    
    float s = (floor(pos.x / period) + 0.5) * period;
    float t = randInRange(s, 0.0, 0.33); // randomizes the line thickness
    float len = period * LINE_LENGTH_RATIO * randInRange(s, 0.667, 1.500); // randomizes the line line length
    pos.y += randInRange(s, -len, len); // randomizes the line y offset
    pos.x += sin(pos.y / len / WOBBLE_LENGTH_RATIO) * period * WOBBLE_WIDTH_RATIO;
    size *= randInRange(s, 0.66, 1.50); // randomizes the time the line pops in
    
    vec2 b = vec2(period, len);
    vec2 p = mod(pos.xy, b);
    vec2 f = (floor(pos.xy / b) + 0.5) * b;
    vec2 o = vec2(randInRange(f.y, -0.025, +0.025) * b.x, randInRange(f.x, -0.1, +0.1) * b.y);
    
    float width  = size;
    float height = len * (1.0 - LINE_GAP_RATIO);
    float ratio  = size / period;
    float val    = drawLine(p, vec4(b / 2.0 + o, width, height));
    float scale  = (ratio > LINE_WIDTH_RATIO) ? (ratio / LINE_WIDTH_RATIO - 1.0) * 2.0 
    			 : (ratio < LINE_WIDTH_RATIO_MIN) ? (1.0 - (ratio - LINE_WIDTH_RATIO_MIN / 2.0) / (LINE_WIDTH_RATIO_MIN / 2.0))
                 : (0.0);
    
	float ret = clamp(val + scale + t, 0.0, 1.0);
    
    // strength determines how much of the lines to draw at this point
    float strength = perlinMixer2(pos, period);
    
	// gives pseudo-aliased edges
	// TODO: make this dependent on pixel size
	return clamp(1.0 - (strength - ret) * 2.0, 0.0, 1.0);
}

float getBanding(vec3 pos, float zoom)
{
    zoom = clamp(zoom, 0.0, 10.0);
	pos = pos * LINE_SCALE;
    
    // scale determines the width of the lines
    float scale = 1.0 / pow(2.0, zoom);
    
    // need about 10 to get the scaling desired for 0.1 (near) to 100.0 (far) scaling
    float val = 1.0;
    for (int i = 0; i < 10; ++i)
        val *= getVal(pos, 8.0 / pow(2.0, float(i)), scale);
    
    return val;
}

float LinearizeDepth(float depth)
{
    float zNear = 0.1;
    float zFar  = 100.0;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

mat4 rotateX(float x)
{
	float s = sin(x);
	float c = cos(x);

	return mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0,   c,  -s, 0.0,
		0.0,   s,   c, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
}

mat4 rotateY(float y)
{
	float s = sin(y);
	float c = cos(y);
	
	return mat4(
		  c, 0.0,   s, 0.0,
		0.0, 1.0, 0.0, 0.0,
		 -s, 0.0,   c, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
}

mat4 rotateZ(float z)
{
	float s = sin(z);
	float c = cos(z);
	
	return mat4(
		  c,  -s, 0.0, 0.0,
		  s,   c, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
}

mat4 rotateZXY(float z, float x, float y)
{
	return rotateZ(z) * rotateX(x) * rotateY(y) * rotateZ(radians(90.0));
}

mat4 calculateAngle(float value)
{
	uint code = uint(value);
	uint zcode = (code & 0xE0) >> 5;
	uint xcode = (code & 0x18) >> 3;
	uint ycode = (code & 0x07) >> 0;

	float zvalue = radians(float(zcode) * 45.0 + 0.0); // 22.5);
	float xvalue = radians(float(xcode) * 22.5 + 0.0);
	float yvalue = radians(float(ycode) * 22.5 + 0.0);

	return rotateZXY(zvalue, yvalue, xvalue);
}

void main()
{
  // Throw away the fragment if less than the draw percentage
  if ((1.0f - order_geom_out) > draw_percentage)
	discard;

  // TODO: manual aliasing
  // 
  // using gl_SamplePosition forces this fragment shader to be ran once per
  // sample instead of once per pixel.
  // 
  //   see: https://forum.unity.com/threads/anti-aliasing-does-not-work-when-shapes-are-drawn-from-fragment-shader.264771/
  // 

  vec3 point = gl_FragCoord.xyz; // + vec3(gl_SamplePosition, 0.0);
  point.x = 2.0f * point.x / viewport_width - 1.0;
  point.y = 2.0f * point.y / viewport_height - 1.0;
  point.y = point.y / ratio;

  vec2 camera_angle = point.xy * radians(40);

  vec3 local_surf_normal = rotate3(vec3(point.y, -point.x, 0), -length(camera_angle)) * norml_geom_out;

  // TODO: pass this direction in through a uniform variable
  vec3 line_proj_dir = normalize(vec3(1.0f, 2.0f, 3.0f));
  vec3 line_draw_dir = normalize(vec3(line_proj_dir.y, -line_proj_dir.x, 0.0f));
  vec3 line_tang_dir = cross(line_proj_dir, line_draw_dir);
  
  // gets the camera direction and 
  vec3 camera_frwd_dir = base_camera_direction;
  vec3 camera_upwd_dir = vec3(0, 0, 1);
  vec3 camera_side_dir = cross(camera_frwd_dir, camera_upwd_dir);
  camera_frwd_dir = rotate3(camera_upwd_dir, camera_angle.x) * rotate3(camera_side_dir, -camera_angle.y) * camera_frwd_dir;

  float zoom_world_normal = 0.0;
  {
    // get zoom difference based on surface-line-dir

	float a = acos(dot(world_normal_geom_out, line_tang_dir));
	zoom_world_normal = -log2(clamp(sin(a), 0.125, 1.0));
  }

  float zoom_camera_normal = 0.0;
  {
	// get zoom difference based on camera-surface angle

	// TODO: this doesn't seem to work quite right, doesn't seem to work for
	// shallow angles slightly off from surface_line_tang_dir

	vec3 surface_line_tang_dir = normalize(line_tang_dir - dot(world_normal_geom_out, line_tang_dir) * world_normal_geom_out);
	float a = acos(dot(camera_frwd_dir, surface_line_tang_dir));
	zoom_camera_normal = log2(clamp(sin(a), 0.015625, 1.0));
  }

  float zoomd = 10.0 - log2(point.z / gl_FragCoord.w * 10.0); // depth zoom
  float zoom = zoomd + zoom_world_normal + zoom_camera_normal;

  vec4 rfpos = reference_projection * reference_view * world_geom_out;

  float val = getBanding(rfpos.yxz, zoom); // TODO: swap x and y elsewhere

  gl_FragColor = vec4(val, val, val, 1.0);
}
