
varying vec3 position;
varying vec3 lightDir, halfVector;
uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;
uniform sampler2D grass_normals;
uniform sampler2D noiseTex;
uniform samplerCube sky;

uniform vec3 eyePos;
uniform vec3 invScale;

uniform float minHeight;
uniform float heightRange;

void main()
{
	vec4 color;

	vec4 groundTexVal = texture2D(groundTex,position.xz * invScale.xz);
	vec3 v = groundTexVal.xyz;
	vec3 n = normalize(vec3(v.x * 2.0 - 1.0, v.y, v.z * 2.0 - 1.0));
	//vec3 t = normalize(cross(n, vec3(0,0,1)));
	//vec3 b = cross(n, t);
	
	float height = minHeight + groundTexVal.a * heightRange;
	if(height < -45.0)
		discard;


	float noiseVal = texture2D(noiseTex, position.xz*0.0025).x + texture2D(LCnoise, position.xz*0.000625).x;
	
	float g = pow(clamp(noiseVal + (height-6.0)/20.0 - 1.5, 0.0, 1.0), 2.0);
	color = mix(texture2D(sand,position.xz*0.0003), texture2D(grass,position.xz*0.00125), g);
	
	color.rgb *= 0.5 + 0.6*texture2D(LCnoise,position.xz*0.02).r;
	//color.a *= clamp(5.0-20.0*((position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5)), 0.0, 1.0);
	
	vec3 normal = n;//normalize( mat3(n,t,-b)*mix(vec3(0,0,1), texture2D(grass_normals, position.xz*0.00125).xyz*2.0 - 1.0, g*g*g));

	
	float NdotL = dot(normal,lightDir);
	
	float m = 1.0 + clamp((height)/fwidth(height),-1.0,0.0)*(0.1-(height)*0.02);
	NdotL *= m;
	color.a *= m;

	vec3 eyeDirection = position.xyz-eyePos;

	color = vec4(mix(color.rgb*max(NdotL,0.5), textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a);
	gl_FragColor = color;
}