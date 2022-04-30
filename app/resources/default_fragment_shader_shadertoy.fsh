/***********************************************************************************
						  AVAILABLE VARIABLES:

vec2 iResolution: visible area {x, y} size in pixels, {0,0} is bottom left
vec4 iMouse: mouse coordinates {x, y} in pixels, {0,0} is bottom left.
	 iMouse.zw is the coordinates of the last mous click.
float iGlobalTime: time in seconds since the shader has started working 
float iTime: an alias for iGlobalTime;
float iTimeDelta: time since the previous frame in seconds

 ***********************************************************************************/

vec3 linearToSRGB(float r, float g, float b)
{
	return vec3(pow(r, 0.45454545), pow(g, 0.45454545), pow(b, 0.45454545)); // 1/2.2
}

vec3 linearToSRGB(vec3 linear_rgb_color)
{
	return linearToSRGB(linear_rgb_color.r, linear_rgb_color.g, linear_rgb_color.b);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	vec2 mousePos = iMouse.xy / iResolution.xy;
	fragColor = vec4(linearToSRGB(uv.x * mousePos.x, uv.y * mousePos.y, 0.5+0.5*sin(iTime)),1.0);
}
