
varying vec3 lightDir, eyeVec;
//varying float rad;
varying vec2 position;
varying vec2 texCoord;
varying float depth;

uniform vec3 eyePos;

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

	n = texture3D(oceanNormals,vec3(texCoord.xy*10.234,0.0002*time)).xyz + texture3D(oceanNormals,vec3(texCoord.xy * 64.0,0.0004*time)).xyz * 0.75;
	n.x = n.x * 2.0 - 1.0;
	n.z = n.z * 2.0 - 1.0;
	n = normalize(n);

	NdotL = dot(n,lightDir);
	
	if (NdotL > 0.0)
		color = vec4(0, 0.1, 0.35, 1.0) + 0.15 * NdotL; // + vec4(10,10,10,0) * pow(max(dot(n,normalize(halfVector)),0.0),60.0);
	else
		color = vec4(0, 0.1, 0.35, 1.0);

		
	
	float facing = (1.0 - NdotL);			
	float fresnel = max(0.2 + (1.0-0.2)*pow(facing, 2.0), 0.0); //0.2 = bias; 2.0 = power <--- these change the water reflections
	// Use distance to lerp between refraction and deep water color
//	float fDistScale = clamp(10.0/depth, 0.0, 1.0);
//	vec3 WaterDeepColor = vec3(0, 0.25, 0.35) * fDistScale + (1 - fDistScale) * color.rgb; //vec3(0, 0.1, 0.125));  
	// Lerp between water color and deep water color
	vec3 waterColor = (vec3(0, 0.1, 0.15) * facing + color.rgb * (1.0 - facing));
	vec3 cReflect = fresnel * textureCube(sky, reflect(eyePos,n));
	color = vec4(cReflect + waterColor, clamp((1.7 - 1.7*r)*(1.7 - 1.7*r)*(1.7 - 1.7*r), 0.0, 1.0));

	gl_FragColor = color;
}