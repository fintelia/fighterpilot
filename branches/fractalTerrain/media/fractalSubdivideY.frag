#version 330

uniform sampler2D tex;
uniform float amplitudeScale;

uniform vec2 origin;

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
    uint h = hash(v.x) ^ hash(v.y);
    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    return uintBitsToFloat(h) - 1.0;
}

vec4 computeValueAtCoord(ivec2 coord)
{
	vec4 value = vec4(1);
	vec3 texVal_0 = texelFetch(tex, coord + ivec2(0,1), 0).rgb;
	vec3 texVal_2 = texelFetch(tex, coord - ivec2(0,1), 0).rgb;
	
	value.xy = 0.5 * (texVal_2.xy + texVal_0.xy);
	value.x += (1.0-2.0*random(origin+coord)) * amplitudeScale;
	value.z = 0.5 + 0.5 * (texVal_2.x - texVal_0.x);
	return clamp(value, vec4(0), vec4(1));
}

void main()
{
	ivec2 coord = ivec2(floor(gl_FragCoord.x), floor(gl_FragCoord.y));
	vec4 outColor = vec4(1);

	ivec2 texSize = textureSize(tex);

	if((coord.y % 2) == 1)
	{
		outColor = computeValueAtCoord(coord);
	}
	else if(coord.x < texSize.x && coord.y < texSize.y)
	{
		vec4 texVal_0 = computeValueAtCoord(coord
		outColor.rgb = texelFetch(tex, coord, 0).rgb;
		outColor.z = 0
	}

	FragColor = clamp(outColor, vec4(0), vec4(1));
}
