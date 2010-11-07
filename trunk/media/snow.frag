
varying vec3 position, lightDir, halfVector;

uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;

void main()
{
	if(position.x < 0.0 || position.x > 1.0 || position.z < 0.0 || position.z > 1.0) discard;

	vec3 normal = texture2D(groundTex,position.xz).xyz;
	normal.x = normal.x * 2.0 - 1.0;
	normal.z = normal.z * 2.0 - 1.0;
	normal = normalize(normal);
	float NdotL = dot(normal,lightDir);


	gl_FragColor = texture2D(snow,position.xz*8.0) * (1.1-texture2D(LCnoise,position.xz*21.435).r*0.2) * (0.95 + clamp(NdotL*0.25,0.0,0.05));
}