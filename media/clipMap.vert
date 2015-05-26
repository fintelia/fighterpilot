#version 330

#extension GL_ARB_texture_gather : require

out vec3 position;
out vec2 texCoord;
out float parentBlend;
out float flogz;

uniform mat4 cameraProjection;
uniform float earthRadius;
uniform vec2 worldOrigin;
uniform float sideLength;
uniform float invResolution;
uniform ivec2 flipAxis = ivec2(0,0);

uniform sampler3D oceanHeights;

uniform sampler2D heightmap;
uniform ivec2 textureOrigin;
uniform int textureStep;
uniform int resolution;

uniform float time;
uniform vec3 eyePosition;

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
    texCoord = vec2(tCoord+0.5) / textureSize(heightmap,0);

    position.y = texelFetch(heightmap, tCoord, 0).x;

    // TODO: use textureGatherOffsets for this
    ivec2 mCoord = iposition % 2;
    ivec2 pCoord = tCoord - mCoord * textureStep;
    vec3 pHeights = vec3(
        texelFetch(heightmap, pCoord + textureStep*ivec2(0,0), 0).x,
        texelFetch(heightmap, pCoord + textureStep*ivec2(2,0), 0).x,
        texelFetch(heightmap, pCoord + textureStep*ivec2(0,2), 0).x);
    const vec3 mask[] = vec3[](vec3(1.0, 0.0, 0.0),
                               vec3(0.5, 0.5, 0.0),
                               vec3(0.5, 0.0, 0.5),
                               vec3(0.0, 0.5, 0.5));
    float parentHeight = dot(pHeights, mask[mCoord.x + 2*mCoord.y]);
    
    vec2 npos = Position2 * invResolution;
    vec2 fpos = npos * (1-flipAxis) + (1 - npos) * flipAxis;
    position.xz = worldOrigin + fpos * sideLength;

    vec2 vSkirt = abs(position.xz - eyePosition.xz) * 2.0 / sideLength;
    parentBlend = clamp(max(vSkirt.x, vSkirt.y) * 5.0 - 3.5, 0.0, 1.0);
    position.y = mix(position.y, parentHeight, parentBlend);
    
//    float r = length(position.xz) / 10000;
//    position.y = 1500 * sin(3.1415 * r) / (3.1415 * r);
    vec2 r = position.xz / earthRadius;

//    position.y = max(position.y, 0.0);

    position.y -= 1150;
    float waterAmount = clamp(1.0 - position.y*0.5, 0, 1);
    float waveHeight = 20.0*(texture(oceanHeights,
                                     vec3(position.xz / 2000.0,time/16))-0.5).r;
    position.y = mix(position.y, waterAmount*waveHeight, waterAmount);
    
    position.y += earthRadius * (sqrt(1.0 - dot(r,r)) - 1.0);


	gl_Position = cameraProjection * vec4(position, 1.0);

	flogz = 1.0 + gl_Position.w;
}
