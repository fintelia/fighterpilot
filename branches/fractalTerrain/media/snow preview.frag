

varying vec3 position;
varying vec3 sunDir;

uniform float time;
uniform float heightScale;

uniform sampler2D snow;
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
	float NdotL = dot(normal,normalize(sunDir));

	gl_FragColor = vec4(texture2D(snow,position.xz*0.0002).rgb * (0.85 + 0.15*NdotL), 1.0);
}