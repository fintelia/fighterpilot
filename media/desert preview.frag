
varying vec3 position;
varying vec3 sunDir;

uniform float heightScale;

uniform sampler2D sand;
uniform sampler2D sand2;
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

	float noiseVal = texture2D(LCnoise, position.xz*0.01).x * 2.5;

	vec3 color;
	color = mix(vec3(texture2D(sand2,position.xz*0.00005).rgb * (noiseVal*0.5+0.5)),
				vec3(texture2D(sand,position.xz*0.00005).rgb * (noiseVal*0.3+0.7)),//,clamp(15.0-15.0*r,0.0,1.0));
				clamp(normal.y*30.0, 0.0, 1.0));

	float r = distance(position.xz*invScale.xz,vec2(0.5,0.5));
	float boundColor = clamp(1.0 - 500.0*abs(r-0.4), 0.0, 1.0);
	if(r > 0.4) boundColor = min(boundColor+0.5, 1.0);

	gl_FragColor = vec4(mix(color*vec3(1.0,1.0,0.7)*(0.5+0.5*NdotL), vec3(1,0,0), boundColor), 1.0);
}