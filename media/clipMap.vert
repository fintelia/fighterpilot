#version 330

out vec3 position;
out vec2 texCoord;
out float flogz;

uniform mat4 cameraProjection;
uniform float earthRadius;
uniform vec2 worldOrigin;
uniform float sideLength;
uniform float invResolution;
uniform ivec2 flipAxis = ivec2(0,0);

uniform sampler2D heightmap;
uniform ivec2 textureOrigin;
uniform int textureStep;
uniform int resolution;

in vec2 Position2;

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

void main()
{
    ivec2 iposition = ivec2(Position2);
    iposition = iposition * (1-flipAxis) + (resolution-iposition) * flipAxis;
    ivec2 tCoord = iposition*textureStep + textureOrigin;
    texCoord = vec2(tCoord) / textureSize(heightmap,0);

    position.y = texelFetch(heightmap, tCoord, 0).x;

    vec2 npos = Position2 * invResolution;
    vec2 fpos = npos * (1-flipAxis) + (1 - npos) * flipAxis;
    position.xz = worldOrigin + fpos * sideLength;

//    float r = length(position.xz) / 10000;
//    position.y = 1500 * sin(3.1415 * r) / (3.1415 * r);
    vec2 r = position.xz / earthRadius;
	position.y = max(position.y, 0.0);
	position.y += earthRadius * (sqrt(1.0 - dot(r,r)) - 1.0);

	gl_Position = cameraProjection * vec4(position, 1.0);

	flogz = 1.0 + gl_Position.w;
}
