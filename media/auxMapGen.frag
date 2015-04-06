#version 330

uniform sampler2D heightmap;
uniform float invTexelSize;

out vec4 FragColor;

void main()
{
    ivec2 texCoord = ivec2(gl_FragCoord.xy-0.5);

    float dydx = 0.5 * invTexelSize * (texelFetch(heightmap, texCoord+ivec2(1,0), 0).x - texelFetch(heightmap, texCoord-ivec2(1,0), 0).x);
    float dydz = 0.5 * invTexelSize * (texelFetch(heightmap, texCoord+ivec2(0,1), 0).x - texelFetch(heightmap, texCoord-ivec2(0,1), 0).x);

    vec3 normalizedNormal = normalize(vec3(dydx,dydz,1)); 
    vec3 scaledNormal = normalizedNormal * vec3(0.5, 0.5, 1) + vec3(0.5, 0.5, 0);
    FragColor = vec4(scaledNormal,1);
}
