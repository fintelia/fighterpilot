

varying vec3 position;
varying vec3 sunDir, sunHalfVector;
varying vec3 lightDirections[4];

uniform float time;

uniform samplerCube sky;
uniform sampler2D groundTex;
uniform sampler2D LCnoise;
uniform sampler2D noiseTex;
uniform sampler2D sand;
uniform sampler2D sand2;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D grass_normals;
uniform sampler2D snow_normals;

uniform vec2 gtex_halfPixel;
uniform vec2 gtex_origin;
uniform vec2 gtex_invScale;

uniform vec3 eyePos;

uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];
uniform float invLightStrengths[4];

uniform float dist;

void main()
{
	//if(position.x < 0.0 || position.x > 1.0 || position.z < 0.0 || position.z > 1.0) discard;

	vec3 n = texture2D(groundTex,gtex_halfPixel + (position.xz-gtex_origin) * gtex_invScale).xyz;
	n.xz = n.xz * 2.0 - 1.0;
	n.y *= 0.1;
	n = normalize(n);
	//vec3 t = normalize(cross(n, vec3(0,0,1)));
	//vec3 b = cross(n, t);
	//vec3 normal = normalize( mat3(n,t,b) * texture2D(snow_normals, position.xz*8.0).xyz*2.0-1.0 );

//	float NdotL = dot(n,normalize(lightDir[0]))*0.5+0.5;
//	for(int i=1;i<numLights;i++)
//	{
//		NdotL += clamp(/* (dot(normal,normalize(lightDir[i]))*0.5+0.5) **/ lightStrength[i] * (50.0-distance(position,lightPosition[i]))*0.02,0.0,1.0);
//	}
	vec3 light = vec3(1,1,0.7) * max(dot(n,normalize(sunDir))*0.5+0.5,0.0);
	for(int i=0;i<4;i++)
	{
		vec3 lightVec = position - lightPositions[i];
		light += lightColors[i] * clamp(0.5 - length(lightVec) * invLightStrengths[i], 0.0,0.5);
	}

	//float r = 0.0;//length(position.xz* invScale.xz - vec2(0.5,0.5)) * 2.0;
	float noiseVal = /*texture2D(LCnoise, position.xz*0.005).x +*/ texture2D(LCnoise, position.xz*0.01).x * 2.5;

	vec3 color;
	color = mix(texture2D(sand2,position.xz*0.0005).rgb * (noiseVal*0.3+0.7) * light,
				texture2D(sand,position.xz*0.0005).rgb * vec3(0.85,1,0.85) * (noiseVal*0.5+0.5)  * (light*0.5+0.5),
				clamp(n.y*4.0-1.5, 0.0, 1.0));

	vec3 eyeDirection = position.xyz-eyePos;

	//color *= 0.4 + 0.6*n.y;

	////////////////////////draw grid////////////////////////
	//vec2 px = position.xz/200.0;
	//vec2 fx = abs(fract(px) - 0.5)*10.0;
	//float i = 1.0 - max(     clamp(fx.x/fwidth(px.x*10.0),0.0,1.0) + clamp(fx.y/fwidth(px.y*10.0),0.0,1.0) - 1.0,     0.0);
	//i += (1.0-max(	clamp(0.15*fx.x/fwidth(px.x*10.0),0.0,1.0) + clamp(0.15*fx.y/fwidth(px.y*10.0),0.0,1.0) - 1.0,     0.0)) * 0.3;
	//color.rgb = vec3(0,0,clamp(i+0.1,0.1,1.0));
	/////////////////////////////////////////////////////////

	gl_FragColor = vec4(mix(color, textureCube(sky, vec3(eyeDirection.x,0,eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),1.0);
}