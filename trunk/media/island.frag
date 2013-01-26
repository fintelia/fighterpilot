
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
uniform sampler2D fractalNormals;

uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];
uniform float lightStrengths[4];

uniform vec3 eyePos;

uniform vec2 gtex_halfPixel;
uniform vec2 gtex_origin;
uniform vec2 gtex_invScale;

uniform float minHeight;
uniform float heightRange;

void main()
{
	vec4 color;

	/////////FRAGMENT ATRRIBUTES////////
	vec4 groundTexVal = texture2D(groundTex,gtex_halfPixel + (position.xz-gtex_origin) * gtex_invScale);
	vec3 v = groundTexVal.xyz;
	vec3 n = normalize(vec3(v.x * 2.0 - 1.0, v.y, v.z * 2.0 - 1.0));
	vec3 t = normalize(cross(n, vec3(0,0,1)));
	vec3 b = cross(n, t);

	float slopeAngle = acos(n.y);
	float slope = sqrt(1.0/(n.y*n.y) - 1.0);

	float height = minHeight + groundTexVal.a * heightRange;

	//height += pow(texture2D(noiseTex, position.xz*0.00125).r,3.0) * 10.0 * slope;

	///////////NORMAL MAPPING//////////
	//n = normalize( mat3(n,t,-b)*mix(vec3(0,0,1), normalize((texture2D(fractalNormals, position.xz*0.0023).xyz*2.0 - 1.0) + 0.8*(texture2D(fractalNormals, position.xz*0.000625).xyz*2.0 - 1.0)), clamp(0.1+(slopeAngle - 0.1) / 0.1, 0.0, 1.0)));

	///////////EARLY DISCARD//////////
	if(height < -45.0)
		discard;

	///////////GRASS AND SAND//////////
	float noiseVal = texture2D(noiseTex, position.xz*0.0025).x + texture2D(LCnoise, position.xz*0.000625).x;
	float gAmount = pow(clamp(noiseVal + (height-6.0)/20.0 - 1.5, 0.0, 1.0), 2.0);
	color = mix(texture2D(sand,position.xz*0.0003), texture2D(grass,position.xz*0.00125), gAmount);

	////////////TREE CANOPY////////////
	color.rgb *= 1.0 - 0.6 * clamp((height - 37.0)*0.2, 0.0, 1.0) * max(clamp((148.0-height)*0.2,0.0,1.0),clamp((slopeAngle - 0.1) / 0.1, 0.0, 1.0));
	//color.a *= clamp(5.0-20.0*((position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5)), 0.0, 1.0);

	//////////////ROCK/////////////////
	float rAmount = /*clamp(height * 0.02, 0.0, 1.0) */ clamp((slopeAngle - 0.40) / 0.10, 0.0,1.0);
	color = mix(color, /*vec4(0.35,0.25,0.20,1.0)*/texture2D(rock,position.xz*0.00125), rAmount);

	/////////LOW CONTRAST NOISE////////
	color.rgb *= 0.5 + 0.6*texture2D(LCnoise,position.xz*0.02).r;




	///////////////LIGHT///////////////
	vec3 light = vec3(1,1,0.7) * (dot(n,normalize(sunDir))*0.5+0.5);
	for(int i=0;i<4;i++)
	{
		vec3 lightVec = position - lightPositions[i];
		light += lightColors[i] * clamp(lightStrengths[i] * 1000.0 / dot(lightVec,lightVec),0.0,0.5);
	}

	////////////WATER EFFECT///////////
	float waterAlpha = clamp(1.0 + clamp((height)/fwidth(height),-1.0,0.0)*(0.1-(height)*0.02), 0.0,1.0);
	light *= waterAlpha;
	color.a *= waterAlpha;

	////////////////FOG////////////////
	vec3 eyeDirection = position.xyz-eyePos;
	color = vec4(mix(color.rgb*max(light,0.7), textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a);

	///////////////GRID////////////////
	//vec2 px = position.xz/1600.0 + 0.5;
	//vec2 fx = abs(fract(px) - 0.5)*10.0 ;
	//color.rgb *= max(     clamp(fx.x/fwidth(px.x*10.0),0.0,1.0) + clamp(fx.y/fwidth(px.y*10.0),0.0,1.0) - 1.0,     0.0);

	////////////TREE SHADOWS///////////
	//if(height > 40.0)
	//{
	//	vec2 tileCenter = floor(position.xz * 0.05);
	//	vec2 center = fract(position.xz * 0.05) - 0.5 * vec2(sin(tileCenter.x+tileCenter.y),cos(tileCenter.x+tileCenter.y)) - vec2(0.5,0.5);
	//	float r = length(center);
	//	color.rgb *= clamp(r+0.6, 0.8, 1.0);
	//}

	////////////COLOR OUTPUT///////////
	gl_FragColor = color; // vec4(vec3(height),1.0);// 
}