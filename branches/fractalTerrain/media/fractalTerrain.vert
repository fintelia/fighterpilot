
varying vec3 position;
varying vec3 tangent;
varying vec3 bitangent;
varying vec3 normal;
varying vec3 sunDir, sunHalfVector;
varying float flogz;
varying vec2 tCoord;
//varying vec3 lightDirections[4];

//uniform vec3 sunPosition;
//uniform vec3 lightPositions[4];
//uniform vec3 eyePos;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;
uniform float earthRadius;

uniform sampler2D groundTex;
uniform vec3 origin;
uniform vec3 scale;

uniform vec2 tOrigin;
uniform vec2 tScale;
uniform float tSize;

attribute vec2 Position2;

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
float random(vec2 v)
{
    uint h = hash(floatBitsToUint(v.x)) ^ hash(floatBitsToUint(v.y*v.y));
    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    
    return uintBitsToFloat(h) - 1.0;
}


float cubic(float t, float x0, float m0, float x1, float m1)
{
	return x0; //todo: implement
}

void main()
{
	vec4 groundVal;
	tCoord = tOrigin + tScale*Position2;
//	ivec2 tFloor = floor(textureSize() * tCoord);
//	ivec2 tCeil = ceil(textureSize() * tCoord);

//	if(tCeil == tFloor)
	{
		groundVal = texture2D(groundTex, tCoord);
	}
//	else
//	{
//		vec4 texels[4] = {
//			texelFetch(groundTex, tFloor),
//			texelFetch(groundTex, vec2(tCeil.x, tFloor.y)),
//			texelFetch(groundTex, tCeil),
//			texelFetch(groundTex, vec2(tFloor.x, tCeil.y)),
//		};
//
//		float h1 = cubic(texels[0].r texels[0].) 
//		groundVal.r = cubic()
//	}

	normal = normalize(scale * vec3((2.0*groundVal.b-1.0),
							1,
							(2.0*groundVal.g-1.0)));

	float displace = 4.0 * random(Position2*scale.xz + origin.xz) - 2.0;

	float y = groundVal.r;
	vec3 unscaledPos = vec3(Position2.x, y, Position2.y);
	vec4 pos = modelTransform * vec4(origin + unscaledPos * scale, 1.0);
//	pos.xz += vec4(normal*(groundVal.a - 0.5) * 2000.0, 0.0).xz;
	position = pos.xyz;
	
	vec2 r = pos.xz / earthRadius;
	pos.y += earthRadius * (sqrt(1.0 - dot(r,r)) - 1.0);
	gl_Position = cameraProjection * pos;

	flogz = 1.0 + gl_Position.w;

//	sunHalfVector = sunPosition + position.xyz;
//	sunDir = normalize(sunPosition/* - position.xyz*/);
}
