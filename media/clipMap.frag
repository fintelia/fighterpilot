#version 330

in vec3 position;
in vec2 texCoord;
in float flogz;

uniform sampler2D heightmap;
uniform sampler2D normalmap;

uniform sampler2D grass;

uniform vec3 sunDirection;
uniform vec3 eyePosition;

out vec4 FragColor;

void main()
{
    float height = texture(heightmap, texCoord).x;
    vec3 normal = normalize(texture(normalmap, texCoord).xzy) * vec3(2,1,2)
        - vec3(1,0,1);
    float slope = length(normal.xz) / normal.y * 3.0;
    float nDotL = dot(normal,sunDirection) * 0.5 + 0.5;

    vec3 color = texture2D(grass, position.xz*0.0005).rgb;
    color = mix(color, vec3(.6, .6, .0), slope * 0.3);
    color = mix(color, vec3(.35, .3, .27), clamp(5*(slope-.8), 0.0, 1.0));

    FragColor = vec4(color*nDotL,1);
	///////////////DEPTH///////////////
	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	const float Fcoef = 2.0 / log2(2000000.0 + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;
	gl_FragDepth = log2(flogz) * Fcoef_half;
}
