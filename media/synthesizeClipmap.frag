#version 330

#extension GL_ARB_texture_gather : require

uniform sampler2D parent_heightmap;
uniform sampler2D parent_normalmap;
uniform ivec2 parent_center;
uniform ivec2 center;

uniform int layerScale;
uniform float texelSize;

uniform int resolution;
uniform int layerResolution;

uniform int vertexStep;

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
float random(uvec2 v)
{
    uint h = hash(hash(v.x) ^ hash(v.y));
    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    return uintBitsToFloat(h) - 1.0;
}

/**
 * Returns the intepolated value and its derivative obtained from doing a cubic
 * interpolation:
 * 
 *    f(-1) = a
 *    f(0)  = b
 *    f(1)  = c
 *    f(2)  = d
 * 
 * Result = vec2(  f(t), f'(t)  )
 *
 * see: http://www.paulinternet.nl/?page=bicubic
 */
vec2 cubic(float a, float b, float c, float d, float t){
    float A = -0.5*a + 1.5*b - 1.5*c + 0.5*d;
    float B =      a - 2.5*b + 2.0*c - 0.5*d;
    float C = -0.5*a         + 0.5*c;
    float D = b;
    
    return vec2(A*t*t*t + B*t*t + C*t + D, 3*A*t*t + 2*B*t + C);
}
void interpolate(vec2 texCoord, out float height, out vec3 normal){
    float invTexSize = 1.0 / layerResolution;

    vec2 nCoord = (texCoord+0.5) * invTexSize;
    vec4 g00 = textureGatherOffset(parent_heightmap, nCoord, ivec2(-1,-1));
    vec4 g10 = textureGatherOffset(parent_heightmap, nCoord, ivec2( 1,-1));
    vec4 g01 = textureGatherOffset(parent_heightmap, nCoord, ivec2(-1, 1));
    vec4 g11 = textureGatherOffset(parent_heightmap, nCoord, ivec2( 1, 1));

    vec2 i = fract(texCoord);
    vec2 h0 = cubic(g00.w, g00.z, g10.w, g10.z, i.x);
    vec2 h1 = cubic(g00.x, g00.y, g10.x, g10.y, i.x);
    vec2 h2 = cubic(g01.w, g01.z, g11.w, g11.z, i.x);
    vec2 h3 = cubic(g01.x, g01.y, g11.x, g11.y, i.x);

    vec2 h = cubic(h0.x, h1.x, h2.x, h3.x, i.y);

    height = h.x;
    normal = normalize(vec3(mix(h1.y, h2.y, i.y), 1, h.y));
}

void main()
{ 
    ivec2 texCoord = ivec2(gl_FragCoord.xy-0.5);
    ivec2 position = center + layerScale * (texCoord - layerResolution/2);

    vec2 parent_texCoord =
        vec2(position - parent_center + layerScale*layerResolution)
        / (layerScale*2);
    
    float height;
    vec3 normal;
    interpolate(parent_texCoord, height, normal);

    if(fract(parent_texCoord) != vec2(0,0)){
        float slope = tan(acos(normal.y));
        float random_scale = clamp(slope-0.02, 0, 0.2) * texelSize;
        height += random_scale * (random(uvec2(position+resolution/2))-0.5);
    }

    float pvHeight = 0;

    FragColor = vec4(height, pvHeight, 0, 1);
}
