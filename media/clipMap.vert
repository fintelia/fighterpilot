#version 330

out vec3 position;
out float flogz;

uniform mat4 cameraProjection;
uniform float earthRadius;
uniform vec2 worldOrigin;
uniform float sideLength;
uniform float invResolution;

uniform sampler2D heightmap;
uniform ivec2 textureOrigin;

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
    ivec2 texCoord = ivec2(mod(ivec2(Position2) + textureOrigin,
                               textureSize(heightmap, 0)));

    position.y = texelFetch(heightmap, texCoord, 0).x;
    position.xz = worldOrigin + abs(Position2 * invResolution*2) * sideLength;

//    float r = length(position.xz) / 10000;
//    position.y = 1500 * sin(3.1415 * r) / (3.1415 * r);
    //vec2 r = position.xz / earthRadius;
	//position.y = max(position.y, 0.0);
	//position.y += earthRadius * (sqrt(1.0 - dot(r,r)) - 1.0);

	gl_Position = cameraProjection * vec4(position, 1.0);

	flogz = 1.0 + gl_Position.w;
}
