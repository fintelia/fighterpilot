
varying vec3 position;
varying vec3 sunDir;

uniform float heightScale;

uniform sampler2D sand;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;

uniform vec3 invScale;

void main()
{
	vec3 normal = texture2D(groundTex,position.xz * invScale.xz).xyz;
	normal.x = normal.x * 2.0 - 1.0;
	normal.z = normal.z * 2.0 - 1.0;
	normal.y /= heightScale;
	normal = normalize(normal);

	float NdotL = dot(normal, normalize(sunDir));

	float noiseVal = texture2D(LCnoise, position.xz*0.0005).x + texture2D(LCnoise, position.xz*0.002).x + texture2D(LCnoise, position.xz*0.006).x;
	gl_FragColor = vec4(texture2D(sand,position.xz*0.000125).rgb * max(NdotL,0.5) * noiseVal, 1.0);
}