

varying vec3 position, lightDir, halfVector;

uniform float time;
uniform float heightScale;

//uniform sampler2D sand;
//uniform sampler2D grass;
//uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;

void main()
{
	if(position.x < 0.0 || position.x > 1.0 || position.z < 0.0 || position.z > 1.0) discard;
	//if(!gl_FrontFacing)
	//{
	//	gl_FragColor = vec4(0.0,0.0,0.0,1.0);		not supported on some hardware
	//	return;
	//}

	vec3 normal = texture2D(groundTex,position.xz).xyz;
	normal.x = normal.x * 2.0 - 1.0;
	normal.z = normal.z * 2.0 - 1.0;
	normal.y /= heightScale;
	normal = normalize(normal);
	float NdotL = dot(normal,lightDir);

	gl_FragColor = vec4(texture2D(snow,position.xz*8.0).rgb /* (1.1-texture2D(LCnoise,position.xz*21.435).r*0.2)*/ * (NdotL*0.3+0.7),1.0);
	//gl_FragColor = vec4(lightDir,1.0);
}