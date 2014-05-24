
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
uniform sampler2D treesTexture;

uniform sampler2D grassDetail;

uniform vec3 lightColors[4];
uniform vec3 lightPositions[4];
uniform float invLightStrengths[4];

uniform vec3 eyePos;

uniform vec2 gtex_halfPixel;
uniform vec2 gtex_origin;
uniform vec2 gtex_invScale;

uniform vec2 ttex_halfPixel;
uniform vec2 ttex_origin;
uniform vec2 ttex_invScale;

uniform float minHeight;
uniform float heightRange;

//see:http://amindforeverprogramming.blogspot.com/2013/07/random-floats-in-glsl-330.html
uint hash(uint x)
{
    x += x << 10u;
    x ^= x >>  6u;
    x += x <<  3u;
    x ^= x >> 11u;
    x += x << 15u;
    return x;
}
float random(vec2 seed, float c1, float c2)
{
    uint h = hash(uint(seed.x * 293.0) + uint(float(seed.y) * c1 + c2));
    h &= 0x007FFFFFu;
    h |= 0x3F800000u;
    
    return uintBitsToFloat(h) - 1.0;
}

//uint randomInt(vec2 seed, float c1, float c2)
//{
//    return hash(uint(seed.x * 293.0) + uint(float(seed.y) * c1 + c2));
//}

vec4 computeColor(float maxBlockSize)
{
	float rowSize = maxBlockSize * 0.002;

	vec2 blockSize = vec2(maxBlockSize);

	for(int i=0; i < 2; i++){
		float rand = random(floor(abs(position.xz) / blockSize)*blockSize, 6365.23, 1652.74);

		if(rand < 0.25)
			break;
		else if(fract(rand * 8.0) < 0.125){
			blockSize.x *= 0.5;
			break;	
		}
		else if(fract(rand * 8.0) < 0.25){
			blockSize.y *= 0.5;
			break;
		}

		blockSize *= 0.5;
	}

	vec2 roundedPosition = floor(abs(position.xz) / blockSize)*blockSize;

	vec3 cropColor = vec3(	0.4+0.6*random(roundedPosition, 2342.23, 563.65),
							0.4+0.6*random(roundedPosition, 635.342, 783.32),
							0.4+0.6*random(roundedPosition, 872.341, 1563.2365));


	vec2 relativePosition = fract(abs(position.xz) / blockSize);

	vec2 edgeDist = abs(relativePosition - vec2(0.5));
	float edgeAlpha = clamp(50.0 - max(edgeDist.x,edgeDist.y) * 100.0, 0.0, 1.0);

	float rand = random(roundedPosition, 3920.2, 46234.3);
	float rowX = rand < 0.5 ? abs(position.x) : abs(position.z);
	float rowAlpha = 0.0;//clamp(1.0 - 2.0 * abs(fract(rowX / rowSize) - 0.5), 0.0, 1.0);

	return vec4(cropColor, min(edgeAlpha, 1.0 - rowAlpha*rowAlpha));	
}

void main()
{
	vec4 color;

	/////////FRAGMENT ATRRIBUTES////////
	vec4 groundTexVal = texture2D(groundTex,gtex_halfPixel + (position.xz-gtex_origin) * gtex_invScale);
	vec3 v = groundTexVal.xyz;
	vec3 n = normalize(vec3(1.0 - v.x * 2.0, v.y, 1.0 - v.z * 2.0));
	vec3 t = normalize(cross(n, vec3(0,0,1)));
	vec3 b = cross(n, t);

	float slopeAngle = acos(n.y);
	float slope = sqrt(1.0/(n.y*n.y) - 1.0);

	float height = minHeight + groundTexVal.a * heightRange;

	//height += pow(texture2D(noiseTex, position.xz*0.00125).r,3.0) * 10.0 * slope;

	///////////NORMAL MAPPING//////////
	//n = normalize( mat3(n,t,-b)*mix(vec3(0,0,1), normalize((texture2D(fractalNormals, position.xz*0.0023).xyz*2.0 - 1.0) + 0.8*(texture2D(fractalNormals, position.xz*0.000625).xyz*2.0 - 1.0)), clamp(0.1+2.0*(slopeAngle - 0.1) / 0.1, 0.0, 1.0)));
	//n = normalize( mat3(t,n,b)*mix(vec3(0,1,0), normalize((texture2D(fractalNormals, position.xz*0.0023).xzy*2.0 - 1.0)), clamp((slopeAngle-0.4)/ 0.1, 0.0, 1.0)));
	//slopeAngle = acos(n.y);
	//slope = sqrt(1.0/(n.y*n.y) - 1.0);

	///////////EARLY DISCARD//////////
	if(height < -45.0)
		discard;



	///////////FARM AND SAND//////////
	float noiseVal = texture2D(noiseTex, position.xz*0.0025).x + texture2D(LCnoise, position.xz*0.000625).x;
	vec3 sandColor =  texture2D(sand,position.xz*0.00125 *4.0).rgb;
	vec4 cropColor = computeColor(200.0) * noiseVal * texture2D(grass,position.xz*0.00125 *1.0);
	cropColor = vec4(mix(sandColor, cropColor.rgb/*vec3(0.2,0.2,0.1) + vec3(0.0, 0.2, 0.0)*/, cropColor.a), 1.0);

	//float noiseVal = texture2D(noiseTex, position.xz*0.0025).x + texture2D(LCnoise, position.xz*0.000625).x;
	//float gAmount = pow(clamp(noiseVal + (height-6.0)/20.0 - 1.5, 0.0, 1.0), 2.0);
	float gAmount = pow(clamp((noiseVal + (height-6.0)/20.0 - 1.5), 0.0, 1.0), 0.5);
	//gAmount = 0.5-0.5*cos(gAmount * 3.141592);
	color = mix(texture2D(sand,position.xz*0.0003), cropColor/*mix(vec4(1,1,1,1), texture2D(grassDetail, position.xz*0.042) * 2.0,clamp(gAmount,0.0,1.0)) * texture2D(grass,position.xz*0.00125 *4.0)*/, gAmount);

	////////////TREE CANOPY////////////
	color.rgb = mix(color.rgb, texture2D(grass,position.xz*0.00125 *4.0).rgb*0.4, clamp((height - 37.0)*0.2, 0.0, 1.0) * max(clamp((148.0-height)*0.5/*0.2*/,0.0,1.0),clamp((slopeAngle - 0.1) / 0.1, 0.0, 1.0)));
	//color.a *= clamp(5.0-20.0*((position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5)), 0.0, 1.0);
	//color.rgb *= 0.8;
	//////////////ROCK/////////////////
	float rAmount = 0; // /*clamp(height * 0.02, 0.0, 1.0) */ clamp((slopeAngle - 0.40) / 0.06, 0.0,1.0);
	color.rgb = mix(color.rgb, vec3(0.15,0.14,0.13)*2.0/*0.5*texture2D(rock,position.xz*0.00125 * 16.0).rgb*/, rAmount); //vec4(0.35,0.25,0.20,1.0)
	//n = normalize( mat3(n,t,-b)*mix(vec3(0,0,1), normalize((texture2D(fractalNormals, position.xz*0.0023).xyz*2.0 - 1.0)/*+ 0.8*(texture2D(fractalNormals, position.xz*0.000625).xyz*2.0 - 1.0)*/), rAmount));

	vec4 fractalColor = texture2D(fractalNormals, position.xz*0.0023);
	color.rgb *= mix(1.0, 0.5+0.5*fractalColor.a, rAmount);
	//vec2 offset = vec2(0,0);//(mat3(t,n,b)*normalize(position - eyePos)).xz * (texture2D(fractalNormals, position.xz*0.0023).a - 0.5) * 0.1;

	vec3 positionToEye = eyePos - position;
	vec3 positionToEye_tangentSpace = mat3(t,n,b)*positionToEye;
	vec2 offset = -normalize(positionToEye_tangentSpace.xz) * (1.0 - fractalColor.a);

	fractalColor = texture2D(fractalNormals, (position.xz+offset)*0.0023 * 0.5);
	n = normalize( mat3(t,n,b)*mix(vec3(0,1,0), normalize(fractalColor.xzy*2.0 - 1.0), rAmount));

	///////////////LIGHT///////////////
	vec4 treesTextureColor = texture2D(treesTexture, ttex_halfPixel + (position.xz-ttex_origin) * ttex_invScale); //tree shadows
	//vec3 light = vec3(1,1,0.7) * max(dot(n,normalize(sunDir))/*0.5+0.5*/ - treesTextureColor.a*0.3, 0.0);
	vec3 light = vec3(1,1,0.7) * (max(dot(n,normalize(sunDir))*0.5+0.5 - treesTextureColor.a*0.3, 0.0));
	light += 0.1 * min(pow(max(dot(n,normalize(sunHalfVector)),0.0),8.0), 1.0);
	for(int i=0;i<4;i++)
	{
		vec3 lightVec = position - lightPositions[i];
		light += lightColors[i] * clamp(0.5 - length(lightVec) * invLightStrengths[i], 0.0,0.5);
	}
	light = clamp(light, 0.0, 1.0);

	////////////WATER EFFECT///////////
	//float waterAlpha = clamp(1.0 + clamp((-height)/fwidth(height),0.0,1.0)*(0.1-0.1*exp(-height*0.1)), 0.0,1.0);
	float waterAlpha = clamp(1.0 + height*0.03, 0.0,1.0);

	//float waterDepth = -height / abs(normalize(positionToEye).y);
	//float waterAlpha = clamp(1.0-0.1*sqrt(waterDepth), 0.0, 1.0);//clamp(1.0 + clamp((waterDepth)/fwidth(waterDepth),0.0,1.0)*(0.1-0.1*exp(-waterDepth*0.1)), 0.0,1.0);
	light *= waterAlpha;
	color.a = clamp(color.a*waterAlpha, 0.0, 1.0);

	////////////TREES TEXTURE//////////
	//float distMult = clamp((950.0*6-distance(eyePos,position))*0.02/6,0.0,1.0);
	//vec4 treesTextureColor = texture2D(treesTexture, ttex_halfPixel + (position.xz-ttex_origin) * ttex_invScale);
	//color.rgb = mix(color.rgb, color.rgb*(1.0-treesTextureColor.a) + treesTextureColor.rgb*0.35, 1.0- distMult*0.5);
	//color.rgb = color.rgb*(1.0-treesTextureColor.a*0.5);// + treesTextureColor.rgb*0.35;

	////////////////FOG////////////////
	vec3 eyeDirection = position.xyz-eyePos;
	color = vec4(mix(color.rgb*max(light,0.0), textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*0.00001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a);

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