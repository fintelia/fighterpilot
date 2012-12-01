
varying vec3 position;
varying vec2 texCoord;

varying vec3 lightDirection;
varying vec3 eyeDirection;

uniform sampler3D oceanNormals;
uniform samplerCube sky;
uniform float time;

void main()
{
	vec3 n;
	vec3 color;
	float NdotL;

	n = texture3D(oceanNormals,vec3(texCoord.xy*6.234,0.0002*time)).xyz + texture3D(oceanNormals,vec3(texCoord.xy * 128.0,0.0004*time)).xyz * 0.75;
	n.xz = n.xz * 2.0 - vec2(1.0,1.0);
	n = normalize(n);
	
	NdotL = dot(n,normalize(lightDirection));

	float power = 2.0;
	float bias = 0.2;
	float fresnel = clamp(bias + (1.0-bias)*pow(1.0 - NdotL, power), 0.0,1.0); //0.0 = bias; 5.0 = power <--- these change the water reflections


	vec3 r = reflect(-normalize(eyeDirection),n);
	r.y = max(r.y,0.0);
	vec3 cReflect = textureCube(sky, r).rgb;

	const vec3 darkColor = vec3(0.08, 0.24, 0.26);
	const vec3 lightColor = vec3(0.12, 0.26, 0.55);

	color = mix(darkColor, lightColor, fresnel);
	color = mix(color, cReflect, fresnel * 0.3 /* *shorelineAlpha*/)   *   (0.6 + 0.4 * NdotL);
	color = mix(color, textureCube(sky, vec3(position.x,0,position.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection), 0.0, 1.0));

	gl_FragColor = vec4(color,1.0);
}
