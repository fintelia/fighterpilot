#version 330

uniform sampler2D tex;
uniform float amplitudeScale;

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
float random(vec2 v)
{
    uint h = hash(uint(floor(v.x* 235235.435) + 345.4*v.y + v.x));
    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    return uintBitsToFloat(h) - 1.0;
}


void main()
{
	ivec2 coord = ivec2(gl_FragCoord.xy);
	vec2 dx = dFdx(texCoord);
	vec2 dy = dFdy(texCoord);
	vec3 value = texture(tex, texCoord).xyz;

	float v0 = texture(tex, texCoord - dx).x;
	float v1 = texture(tex, texCoord + dx).x;
	float v2 = texture(tex, texCoord - dy).x;
	float v3 = texture(tex, texCoord + dy).x;
	value.y = 0.5 + 0.5 * (v1 - v0);
	value.z = 0.5 + 0.5 * (v3 - v2);

//	if(coord.x % 2 == 1 || coord.x % 2 == 0)
//	{
//		color.r += clamp((1.0-2.0*random(gl_FragCoord.xy)) * amplitudeScale,0,1);
//	}

	FragColor = vec4(value, 0.5);
}
