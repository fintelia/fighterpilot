

varying vec3 position;
varying vec3 sunDir, sunHalfVector;
varying vec3 lightDirections[4];

uniform float time;

//uniform sampler2D sand;
//uniform sampler2D grass;
//uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;
uniform sampler2D snow_normals;

uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];
uniform float lightStrengths[4];
//uniform int numLights = 0;

void main()
{
	if(position.x < 0.0 || position.x > 1.0 || position.z < 0.0 || position.z > 1.0) discard;
	//if(!gl_FrontFacing)
	//{
	//	gl_FragColor = vec4(0.0,0.0,0.0,1.0);		not supported on some hardware
	//	return;
	//}

	vec3 n = texture2D(groundTex,position.xz).xyz;
	n.x = n.x * 2.0 - 1.0;
	n.z = n.z * 2.0 - 1.0;
	n = normalize(n);
	vec3 t = normalize(cross(n, vec3(0,0,1)));
	vec3 b = cross(n, t);
	vec3 normal = normalize( mat3(n,t,b) * texture2D(snow_normals, position.xz*8.0).xyz*2.0-1.0 );

	//float NdotL = dot(n,lightDir);

	vec3 light = vec3(1,1,0.7) * (dot(n,normalize(sunDir))*0.5+0.5);
	for(int i=0;i<4;i++)
	{
		vec3 lightVec = position - lightPositions[i];
		light += lightColors[i] * clamp(lightStrengths[i] * 1000.0 / dot(lightVec,lightVec),0.0,0.5);
	}

	float r = length(position.xz - vec2(0.5,0.5)) * 2.0;

	gl_FragColor = //vec4(NdotL*0.7+0.3,NdotL*0.7+0.3,NdotL*0.7+0.3,1.0);
	vec4(texture2D(snow,position.xz*8.0).rgb /* (1.1-texture2D(LCnoise,position.xz*21.435).r*0.2)*/ * (light*0.3+0.7),clamp(5.0-5.0*r,0.0,1.0));
}