
//uniform sampler2D tex;
uniform sampler2D blurTex;
varying float g;
varying vec2 texCoord;

void main()
{
	vec3 color = texture2D(blurTex, texCoord).rgb;
	float intensity = dot(color.rgb,vec3(0.2989, 0.5870, 0.1140));
	color = 0.4*mix(color.rgb, vec3(intensity), 0.9);
	color = pow(color,vec3(g,g,g));
	gl_FragColor = vec4(color, 1.0);
}