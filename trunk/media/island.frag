
varying vec3 position;
varying vec3 sunDir, sunHalfVector;
varying vec3 lightDirections[4];

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


uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];
uniform float lightStrengths[4];
//uniform int numLights = 0;

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


	color.rgb *= 1.0 - 0.6*clamp((height - 37.0)*0.2, 0.0, 1.0);
	//color.a *= clamp(5.0-20.0*((position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5)), 0.0, 1.0);
	
	vec3 normal = n;//normalize( mat3(n,t,-b)*mix(vec3(0,0,1), texture2D(grass_normals, position.xz*0.00125).xyz*2.0 - 1.0, g*g*g));

	
	//float NdotL = max((dot(normal,lightDir[0])*0.5+0.5)*0.5,0.7);
	//for(int i=1;i<numLights;i++)
	//{
	//	NdotL += clamp(/* (dot(normal,normalize(lightDir[i]))*0.5+0.5) **/ lightStrength[i] * (50.0-distance(position,lightPosition[i]))*0.02,0.0,1.0);
	//}
	vec3 light = vec3(1,1,0.7) * (dot(n,normalize(sunDir))*0.5+0.5);
	for(int i=0;i<4;i++)
	{
		vec3 lightVec = position - lightPositions[i];
		light += lightColors[i] * clamp(lightStrengths[i] * 1000.0 / dot(lightVec,lightVec),0.0,0.5);
	}

	float m = clamp(1.0 + clamp((height)/fwidth(height),-1.0,0.0)*(0.1-(height)*0.02), 0.0,1.0);
	light *= m;
	color.a *= m;

	vec3 eyeDirection = position.xyz-eyePos;

	color = vec4(mix(color.rgb*max(light,0.7), textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a);

	///////////////GRID////////////////
	//vec2 px = position.xz/1600.0 + 0.5;
	//vec2 fx = abs(fract(px) - 0.5)*10.0;
	//color.rgb *= max(     clamp(fx.x/fwidth(px.x*10.0),0.0,1.0) + clamp(fx.y/fwidth(px.y*10.0),0.0,1.0) - 1.0,     0.0);
	//////////////////////////////////

	gl_FragColor = color;
}