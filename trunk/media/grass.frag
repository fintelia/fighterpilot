
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


uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];
uniform float lightStrengths[4];
//uniform int numLights = 0;

uniform vec3 eyePos;

uniform vec2 gtex_halfPixel;
uniform vec2 gtex_origin;
uniform vec2 gtex_invScale;

uniform float minHeight;
uniform float heightRange;


void main()
{
	vec4 color = vec4(1,1,1,1);

	vec4 groundTexVal = texture2D(groundTex,gtex_halfPixel + (position.xz-gtex_origin) * gtex_invScale);
	vec3 v = groundTexVal.xyz;
	vec3 normal = normalize(vec3(v.x * 2.0 - 1.0, v.y, v.z * 2.0 - 1.0));
	
	float height = minHeight + groundTexVal.a * heightRange;
	//if(height < -45.0)
	//	discard;
	float slope = acos(dot(vec3(0.0,1.0,0.0),normal));



	//float r=0.0;
	//float s1=0.17;//1.53
	//float s2=0.15;//1.48
	//if(slope>s1 		) 	r=1.0;
	//else if(slope>s2 	)	r=(slope-s2)/(s1-s2);
	//r = clamp((0.989-normal.y) / (0.001), 0.0, 1.0);



	float noiseVal = texture2D(noiseTex, position.xz*0.0025).x + texture2D(LCnoise, position.xz*0.000625).x;
	
	float gMult=0.0;

	float forestMax = 0.998;
	float forestMin = 0.989;

	gMult = clamp((11.0 - 10.0*abs(normal.y - 0.5*(forestMax+forestMin)) / (0.5*(forestMax-forestMin))) * clamp((height - 37.0)*0.2, 0.0, 1.0), 0.0, 1.0);

	//float g = pow(clamp(noiseVal + (height-6.0)/20.0 - 1.5, 0.0, 1.0), 2.0);
	vec3 grassColor = texture2D(grass,position.xz*0.00125).rgb /* (1.0 - 0.6*clamp((height - 37.0)*0.2, 0.0, 1.0))*/*(0.7 - 0.36*gMult);
	//vec3 rockColor = texture2D(rock,position.xz*0.002).rgb;

	color.rgb = grassColor;//mix(grassColor, rockColor, r);
	color.rgb *= 0.5 + 0.6*texture2D(LCnoise,position.xz*0.02).r;


	vec3 light = vec3(1,1,0.7) * (dot(normal,normalize(sunDir))*0.5+0.5);
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

	gl_FragColor = color;
}