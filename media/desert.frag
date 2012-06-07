

varying vec3 position, lightDir, halfVector;

uniform float time;

uniform sampler2D sand;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;
uniform samplerCube sky;
uniform vec3 invScale;
uniform vec3 eyePos;


uniform float dist;

void main()
{
	//if(position.x < 0.0 || position.x > 1.0 || position.z < 0.0 || position.z > 1.0) discard;

	vec3 n = texture2D(groundTex,position.xz * invScale.xz).xyz;
	n.x = n.x * 2.0 - 1.0;
	n.z = n.z * 2.0 - 1.0;
	n.y *= 0.1;
	n = normalize(n);
	//vec3 t = normalize(cross(n, vec3(0,0,1)));
	//vec3 b = cross(n, t);
	//vec3 normal = normalize( mat3(n,t,b) * texture2D(snow_normals, position.xz*8.0).xyz*2.0-1.0 );

	float NdotL = dot(n,lightDir);

	//float r = 0.0;//length(position.xz* invScale.xz - vec2(0.5,0.5)) * 2.0;
	float noiseVal = texture2D(LCnoise, position.xz*0.0005).x + texture2D(LCnoise, position.xz*0.001).x;

	vec3 color = vec3(texture2D(sand,position.xz*0.000125).rgb * NdotL * noiseVal);//,clamp(15.0-15.0*r,0.0,1.0));

	vec3 eyeDirection = position.xyz-eyePos;
	//color.a = 1.0-clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0);


	gl_FragColor = vec4(mix(color*NdotL, textureCube(sky, vec3(eyeDirection.x,0,eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),1.0);
}