
centroid varying vec3 position;
varying vec3 normal;
//varying vec4 groundVal;
//varying vec3 sunDir, sunHalfVector;
//varying vec2 tCoord;
varying float flogz;
//varying vec3 lightDirections[4];

//uniform vec3 sunPosition;
//uniform vec3 lightPositions[4];
//uniform vec3 eyePos;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;
uniform float earthRadius;
//uniform sampler2D groundTex;
//uniform sampler2D waves;
//uniform vec3 origin;
//uniform vec3 scale;

//uniform vec2 tOrigin;
//uniform vec2 tScale;
//uniform float tSize;

//uniform float time;

uniform sampler2D waveTexture;
uniform float invTextureScale;

// const int NUM_WAVES = 16;
// uniform float amplitudes[NUM_WAVES];
// uniform float frequencies[NUM_WAVES];
// uniform float waveSpeeds[NUM_WAVES];
// uniform vec2 waveDirections[NUM_WAVES];


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
//	tCoord = tOrigin + tScale*Position2;
//	ivec2 tFloor = floor(textureSize() * tCoord);
//	ivec2 tCeil = ceil(textureSize() * tCoord);

//	if(tCeil == tFloor)
//	{
//		groundVal = texture2D(groundTex, tCoord);
//	}
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

	// normal = normalize(scale * vec3((2.0*groundVal.b-1.0),
	// 						1,
	// 						(2.0*groundVal.g-1.0)));


	//float y = groundVal.r;
	vec4 pos = modelTransform * vec4(Position2.x, 0, Position2.y, 1.0);

	float A[4] = float[4](1.0, 0.8, 0.4, 0.3);
	float f[4] = float[4](0.0015, 0.0035, 0.008, 0.0021);
	vec2 D[4] = vec2[4](vec2(1,0), vec2(0.6,-0.8), vec2(0,1), vec2(-0.8,-0.6));

//	const float earthRadius = 3.3675e6;
	normal = normalize(pos.xyz + vec3(0, earthRadius, 0));
	vec2 r = pos.xz / earthRadius;
	pos.y = earthRadius * (sqrt(1.0 - dot(r,r)) - 1.0);
	pos.y += (texture2D(waveTexture, pos.xz*invTextureScale*0.1).x-0.5) * 16;
	
//	for(int i=0; i < 4; i++){
//		pos.xyz += normal * A[i]*(texture(waves, vec2(fract(0.0005*time + f[i]*dot(pos.xz,D[i])), 1)).r-0.5);
//	}

//	for(int i=0; i < NUM_WAVES; i++){
//		pos.xyz += normal * amplitudes[i] *
//			(0.5 + 0.5*cos((frequencies[i] * dot(pos.xz,waveDirections[i])
//							+ 0.01*waveSpeeds[i]*time) / (2*3.14159265)));
//	}


	
	

//	pos += vec4(normal*displace, 0.0);

/*	pos.y = 10.0+10.0*cos(0.0005*time+0.01*pos.x);
	pos.x -= 0.5 * 10.0*sin(0.0005*time+0.01*pos.x);

	pos.y = 8.0+8.0*cos(0.0008*time+0.01*dot(pos.xz, vec2(0.6,0.8)));
	pos.xz -= vec2(0.6, 0.8) * 0.5 * 8.0*sin(0.0008*time+0.01*dot(pos.xz, vec2(0.6,0.8)));

	pos.y = 6.0+6.0*cos(0.0015*time+0.01*dot(pos.xz, vec2(-0.8,0.6)));
	pos.xz -= vec2(-0.8, 0.6) * 0.5 * 6.0*sin(0.0015*time+0.01*dot(pos.xz, vec2(0.6,0.8)));*/

	position = pos.xyz;
	gl_Position = cameraProjection * pos;

	flogz = 1.0 + gl_Position.w;

//	sunHalfVector = sunPosition + position.xyz;
//	sunDir = normalize(sunPosition/* - position.xyz*/);
}
