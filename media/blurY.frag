
#version 330

uniform sampler2D tex;
uniform int mipmapLayer;

in vec2 texCoord;
out vec4 FragColor;

const int radius = 9;
const float stddev = 4.0;

void main()
{
    ivec2 coord = ivec2(texCoord * textureSize(tex, mipmapLayer));
    vec3 color = vec3(0);
    float totalWeight = 0;
    for(int i = -radius; i <= radius; i++){
        vec3 texel = texelFetch(tex, coord + ivec2(0, i), mipmapLayer).rgb;
      // vec3 texel =
      //     textureLod(tex, texCoord + vec2(i * pxSize, 0), mipmapLayer).rgb;
        float weight = exp(-0.5 * i*i / (stddev * stddev));
        color += texel * weight;
        totalWeight += weight;
    }
    color /= totalWeight;

	// float pxSize = dFdy(texCoord.y);
	// vec3 color = textureLod(tex, texCoord, mipmapLayer).rgb * 0.2270270270;
	// color += textureLod(tex, texCoord + vec2(0.0, 1.3846153846*pxSize), mipmapLayer).rgb * 0.3162162162;
	// color += textureLod(tex, texCoord - vec2(0.0, 1.3846153846*pxSize), mipmapLayer).rgb * 0.3162162162;	
	// color += textureLod(tex, texCoord + vec2(0.0, 3.2307692308*pxSize), mipmapLayer).rgb * 0.0702702703;
	// color += textureLod(tex, texCoord - vec2(0.0, 3.2307692308*pxSize), mipmapLayer).rgb * 0.0702702703;
	FragColor = vec4(color,1.0);
}
