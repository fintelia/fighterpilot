#version 330

uniform sampler2D tex;
uniform ivec2 textureOrigin;

uniform ivec2 origin;

uniform float levelScale;
uniform float invLevelScale;

in vec2 texCoord;
out vec4 FragColor;

//see:http://amindforeverprogramming.blogspot.com/2013/07/random-floats-in-glsl-330.html
uint hash(uint x)
{
    x += x << 10u;
    x ^= x >>  6u;
    x += x <<  3u;
    x ^= x >> 11u;
    x += x << 15u;
    return x;
}
float random(ivec2 v)
{
    uint h = hash(hash(uint(v.x)) ^ hash(uint(v.y)));
    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    return uintBitsToFloat(h) - 1.0;
}

/**
 * Arguments and return value are in the form vec2(p, m). In other words the x
 * term represents the height, and the y term represents the slope.
 */
vec2 cubic(vec2 p0, vec2 p1, float t)
{
	float t2 = t*t;
	float t3 = t2*t;
	float p = (2.0*t3 - 3.0*t2 + 1) * p0.x + (t3 - 2.0*t2 + t) * p0.y +
		(-2.0*t3 + 3.0*t2) * p1.x + (t3 - t2) * p1.y;

	float m = (6.0*t2 - 6.0*t) * p0.x + (3*t2 - 4*t + 1) * p0.y +
		(-6.0*t2 + 6.0*t) * p1.x + (3*t2 - 2*t) * p1.y;

	return vec2(p, m);
}

vec2 textureCubic(vec2 p0, vec2 p1, float t)
{
	vec2 v0 = p0;
	vec2 v1 = p1;
	v0.y = (p0.y - 0.5) * levelScale;
	v1.y = (p1.y - 0.5) * levelScale;

	vec2 r = cubic(v0, v1, t);// * invTexToWorld;
//	r.y = 0.5 + r.y * invLevelScale * 0.5;

//	float trueSlope = (p1 - p0).x;
//	r.x = 0.5 * (p0+p1).x;
//	r.y = trueSlope *0.5 * invLevelScale+ 0.5;
	r.y = 0.5 + r.y * invLevelScale;

	return r;
}
float randomOffset(vec4 value, ivec2 originPlusCoord)
{
	float s = clamp(30.0*max(abs(value.y-0.5), abs(value.z-0.5)) - 1.0, 0, 1);
	return (random(originPlusCoord) - 0.5) * levelScale * 0.05 * s;
}
/*float randomDisplace(vec4 value, ivec2 originPlusCoord)
{
	float s = clamp(30.0*max(abs(value.y-0.5), abs(value.z-0.5)) - 1.0, 0, 1);
	return random(originPlusCoord) * levelScale * 0.05 * s;
}*/

vec4 valueAtCoord(ivec2 coord)
{
	vec4 value = vec4(0,0,0,0.5);

	if((coord.x % 2) == 0 && (coord.y % 2) == 0)
	{
		value = texelFetch(tex, textureOrigin+coord/2, 0);
	}
	else if((coord.x % 2) == 1 && (coord.y % 2) == 0)
	{
		vec4 texVal_0 = texelFetch(tex,(textureOrigin+coord/2), 0);
		vec4 texVal_1 = texelFetch(tex,(textureOrigin+coord/2)+ivec2(1,0),0);
		value.xy = textureCubic(texVal_0.xy, texVal_1.xy, 0.5);
		value.z = 0.5*(texVal_0+texVal_1).z;

		float h0 = randomOffset(value, origin+coord-ivec2(0,1));
		float h1 = randomOffset(value, origin+coord+ivec2(0,1));
		value.z += (h1 - h0)*0.25*invLevelScale;

		value.x += randomOffset(value, origin+coord);
//		value.w = 0.5*(texVal_0+texVal_1).w + randomDisplace(value, origin+coord);
	}
	else if((coord.x % 2) == 0 && (coord.y % 2) == 1)
	{
		vec4 texVal_0 = texelFetch(tex, textureOrigin+coord/2, 0);
		vec4 texVal_1 = texelFetch(tex, textureOrigin+coord/2+ivec2(0,1), 0);
		value.xz = textureCubic(texVal_0.xz, texVal_1.xz, 0.5);
		value.y = 0.5*(texVal_0+texVal_1).y;

		float h0 = randomOffset(value, origin+coord-ivec2(1,0));
		float h1 = randomOffset(value, origin+coord+ivec2(1,0));
		value.y += (h1 - h0)*0.25*invLevelScale;

		value.x += randomOffset(value, origin+coord);
//		value.w = 0.5*(texVal_0+texVal_1).w + randomDisplace(value, origin+coord);
	}
	else
	{
		vec4 t0 = texelFetch(tex,(textureOrigin+coord/2),0);
		vec4 t1 = texelFetch(tex,(textureOrigin+coord/2)+ivec2(1,0),0);
		vec4 t2 = texelFetch(tex,(textureOrigin+coord/2)+ivec2(0,1),0);
		vec4 t3 = texelFetch(tex,(textureOrigin+coord/2)+ivec2(1,1),0);

		vec2 p0 = textureCubic(t0.xy, t1.xy, 0.5);
		vec2 p1 = textureCubic(t2.xy, t3.xy, 0.5);
		float m0 = 0.5*(t0+t1).z;
		float m1 = 0.5*(t2+t3).z;

		value.xz = textureCubic(vec2(p0.x, m0), vec2(p1.x, m1), 0.5);
		value.y = 0.5*(p0+p1).y;

		//p0.x += randomOffset(vec4(p0,m0,1), origin+coord-ivec2(0,1));
		//p1.x += randomOffset(vec4(p0,m0,1), origin+coord+ivec2(0,1));

		float h0 = randomOffset(value, origin+coord-ivec2(1,0));
		float h1 = randomOffset(value, origin+coord+ivec2(1,0));
		float h2 = randomOffset(value, origin+coord-ivec2(0,1));
		float h3 = randomOffset(value, origin+coord+ivec2(0,1));
		value.y += (h1 - h0)*0.25*invLevelScale;
		value.z += (h3 - h2)*0.25*invLevelScale;


		value.x += randomOffset(value, origin+coord);
//		value.w = 0.25*(t0+t1+t2+t3).w + randomDisplace(value, origin+coord);
	}
	return value;
}
/**
 * value is the value found from valueAtcoord, and coord is the coordinate of
 * the point, including the origin offset.
 */
vec4 fractalValueAtCoord(vec4 value, ivec2 coord)
{
	return vec4(0);
}

void main()
{
	ivec2 coord = ivec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y));
	vec4 outColor = valueAtCoord(coord);

//	outColor += fractalValueAtCoord(outColor, origin+coord);
	FragColor = clamp(outColor, vec4(0), vec4(1));
}
