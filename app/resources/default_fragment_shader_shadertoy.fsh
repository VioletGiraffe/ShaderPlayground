// Complex Number math by julesb
// https://github.com/julesb/glsl-util

#define PI 3.14159265

#define cx_mul(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x)
#define cx_div(a, b) vec2(((a.x*b.x+a.y*b.y)/(b.x*b.x+b.y*b.y)),((a.y*b.x-a.x*b.y)/(b.x*b.x+b.y*b.y)))
#define cx_modulus(a) length(a)
#define cx_conj(a) vec2(a.x,-a.y)
#define cx_arg(a) atan2(a.y,a.x)
#define cx_sin(a) vec2(sin(a.x) * cosh(a.y), cos(a.x) * sinh(a.y))
#define cx_cos(a) vec2(cos(a.x) * cosh(a.y), -sin(a.x) * sinh(a.y))

vec2 cx_sqrt(vec2 a) {
	float r = sqrt(a.x*a.x+a.y*a.y);
	float rpart = sqrt(0.5*(r+a.x));
	float ipart = sqrt(0.5*(r-a.x));
	if (a.y < 0.0) ipart = -ipart;
	return vec2(rpart,ipart);
}

vec2 cx_tan(vec2 a) {return cx_div(cx_sin(a), cx_cos(a)); }

vec2 cx_log(vec2 a) {
	float rpart = sqrt((a.x*a.x)+(a.y*a.y));
	float ipart = atan(a.y,a.x);
	if (ipart > PI) ipart=ipart-(2.0*PI);
	return vec2(log(rpart),ipart);
}

vec2 cx_mobius(vec2 a) {
	vec2 c1 = a - vec2(1.0,0.0);
	vec2 c2 = a + vec2(1.0,0.0);
	return cx_div(c1, c2);
}

vec2 cx_z_plus_one_over_z(vec2 a) {
	return a + cx_div(vec2(1.0,0.0), a);
}

vec2 cx_z_squared_plus_c(vec2 z, vec2 c) {
	return cx_mul(z, z) + c;
}

vec2 cx_sin_of_one_over_z(vec2 z) {
	return cx_sin(cx_div(vec2(1.0,0.0), z));
}


////////////////////////////////////////////////////////////
// end Complex Number math by julesb
////////////////////////////////////////////////////////////


// From Stackoveflow
// http://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl
vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// My own additions to complex number math
#define cx_sub(a, b) vec2(a.x - b.x, a.y - b.y)
#define cx_add(a, b) vec2(a.x + b.x, a.y + b.y)

vec2 cx_to_polar(vec2 a) {
  float phi = atan(a.x, a.y);
  float r = sqrt(a.x * a.x + a.y * a.y);
  return vec2(r, phi);
}


// End utils, here comes the actual fractal

// z^3 - 1
vec2 f(vec2 z) {
  vec2 z3 = cx_mul(z, cx_mul(z, z));
  return vec2(z3.x - 1.0, z3.y);
}

// f(z) derivated
// 3z^2
vec2 fPrim(vec2 z) {
  vec2 z2 = cx_mul(z, z);
  return vec2(3.0*z2.x, 3.0*z2.y);
}


int maxIterations = 300;
vec3 newtonRapson(vec2 z) {
  vec2 oldZ = z;
  int iterations = 0;
  for(int i = 0; i < maxIterations; i++){
	z = cx_sub(z, cx_div(f(z), fPrim(z)));
	if(abs(oldZ.x - z.x) < 0.001 && abs(oldZ.y - z.y) < 0.001) {
	  break;
	}
	oldZ = z;
	iterations++;
  }
  return vec3(float(iterations), cx_to_polar(z));
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	float zoom = (sin(iGlobalTime/2.0)*0.5+0.5)*9.0+0.1;
	vec2 scaled = zoom*(uv*4.0-2.0);
	float cs = cos(iGlobalTime/2.0);
	float sn = sin(iGlobalTime/2.0);
	vec2 rotated = vec2(scaled.x * cs - scaled.y * sn, scaled.x * sn + scaled.y * cs);
	vec3 result = newtonRapson(rotated);
	vec3 color = hsv2rgb(vec3(result.z*1.3, 1.5, 1.0));

	// "It is not, for now, mandatory but recommended to leave the alpha channel to 1.0."
	//float alpha = result.x/float(maxIterations);
	fragColor = vec4(color, 1.0);
}
