#version 330

in vec3 position;
in vec2 texCoord;

uniform sampler2D heightmap;
uniform sampler2D normalmap;
uniform sampler2D grass;
uniform sampler2D LCnoise;

uniform vec3 sunDirection;
uniform int layer;

out vec4 FragColor;

void main()
{
    float height = texture(heightmap, texCoord).x;
    vec3 normal = normalize(texture(normalmap, texCoord).xzy * vec3(2,1,2)
                            - vec3(1,0,1));

    float slope = length(normal.xz) / normal.y;
    float nDotL = max(dot(normal,normalize(sunDirection)), 0);

	vec3 grass_color =
	    vec3(0.55, 0.45, 0.2) * (2 * texture(grass, position.xz * 0.001).rgb);
	vec3 rock_color = vec3(0.45, 0.4, 0.2);
    vec3 color = mix(grass_color, rock_color, clamp(5*(slope-0.05), 0.0, 1.0));
    color *= 1.0 + 0.6*(texture(LCnoise, position.xz*0.025).r +
                        texture(LCnoise, position.xz*0.25).r - 1.0);
    color *= nDotL;

	// Tree coverage
	vec3 foliageColor = vec3(0.08, 0.10, 0.01);
	float density = clamp(0.05 * (height - 10.0 - 1150.0), 0.0, 1.0);
	color = mix(color, foliageColor, density);

	FragColor = vec4(color, 1);
}
