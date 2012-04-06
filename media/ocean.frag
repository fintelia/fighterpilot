
varying vec3 lightDir, eyeVec;
//varying float rad;
varying vec2 position;
varying vec2 texCoord;
varying float depth;

varying vec3 halfVector;
varying vec3 eyeDirection;

uniform sampler3D oceanNormals;
uniform samplerCube sky;
uniform float time, seaLevel;
void main()
{
	vec3 n;
	float NdotL;
	vec4 color;
	float r = length(position);


	//float alphaMult = dot(normalize(eyePos-pos),vec3(0.0,1.0,0.0));

	n = texture3D(oceanNormals,vec3(texCoord.xy*6.234,0.0002*time)).xyz + texture3D(oceanNormals,vec3(texCoord.xy * 128.0,0.0004*time)).xyz * 0.75;
	n.x = n.x * 2.0 - 1.0;
	n.z = n.z * 2.0 - 1.0;
	n = normalize(n);

	NdotL = dot(n,lightDir);
	
	if (NdotL > 0.0)
		color = vec4(0.04, 0.12, 0.13, 1.0) + 0.05 * NdotL;// + vec4(10,10,10,0) * pow(max(dot(n,normalize(halfVector)),0.0),10.0);//vec4(0, 0.1, 0.35, 1.0)
	else
		color = vec4(0.04, 0.12, 0.13, 1.0);
	




	
	//float facing = (1.0 - NdotL);			
	float fresnel = clamp(0.2 + (1.0-0.2)*pow(1.0 - NdotL, 2.0), 0.0,1.0); //0.0 = bias; 5.0 = power <--- these change the water reflections
	//// Use distance to lerp between refraction and deep water color
	////float fDistScale = clamp(10.0/depth, 0.0, 1.0);
	////vec3 WaterDeepColor = vec3(0, 0.25, 0.35) * fDistScale + (1 - fDistScale) * color.rgb; //vec3(0, 0.1, 0.125));  
	//// Lerp between water color and deep water color
	//vec3 waterColor = (vec3(0, 0.1, 0.15) * facing + color.rgb * (1.0 - facing));
	vec3 cReflect = textureCube(sky, reflect(normalize(eyeDirection),n));
	////color = vec4(cReflect + waterColor,1.0);// clamp((1.7 - 1.7*r)*(1.7 - 1.7*r)*(1.7 - 1.7*r), 0.0, 1.0));
	//
	//color = vec4(fresnel * cReflect + waterColor, 1.0);
	


	vec3 darkColor = vec3(0.08, 0.24, 0.26);
	vec3 lightColor = vec3(0.12, 0.26, 0.55);
	color.rgb = mix(darkColor.rgb, lightColor.rgb, fresnel);
    color.rgb *= 0.6 + 0.4 * clamp(dot(lightDir, n),0.0,1.0);

	color.rgb = mix(color.rgb, cReflect, fresnel * 0.3 /* *shorelineAlpha*/);




	float depth = gl_FragCoord.z / gl_FragCoord.w;
	//float t = clamp(0.000000001*depth*depth, 0.0, 1.0);//clamp(1.0 - (2.7 - 2.7*r)*(2.7 - 2.7*r)*(2.7 - 2.7*r), 0.0, 1.0);
	color.rgb = mix(color.rgb, textureCube(sky, vec3(position.x,0,position.y)).rgb, clamp(0.000000002*dot(eyeDirection,eyeDirection), 0.0, 1.0));
	gl_FragColor = color;
}