
centroid varying vec3 position;
//varying vec4 groundVal;
varying vec3 normal;
//varying vec2 tCoord;
invariant varying float flogz;

//uniform vec3 lightColors[4];
//uniform vec3 lightPositions[4];
//uniform float invLightStrengths[4];
//uniform sampler3D oceanNormals;
uniform sampler2D waveTexture;
//uniform sampler2D colorBuffer;
//uniform sampler2D depthBuffer;
//uniform sampler2D waves;
uniform samplerCube sky;

uniform float invTextureScale;
uniform vec2 invScreenDims;
//uniform float slopeScale;
uniform vec3 sunDirection;
uniform vec3 eyePosition;

uniform float colorMult;

//uniform float time;

// const int NUM_WAVES = 16;
// uniform float amplitudes[NUM_WAVES];
// uniform float frequencies[NUM_WAVES];
// uniform float waveSpeeds[NUM_WAVES];
// uniform vec2 waveDirections[NUM_WAVES];



//uniform sampler2D groundTex;
//uniform vec3 origin;
//uniform vec3 scale;

const float PI = 3.14159265;

mat3 getTangentMatrix()
{
	vec3 n = normalize(normal);                 // 0, 1, 0
	vec3 t = normalize(cross(n, vec3(1,0,0)));  // 0, 0, 1
	vec3 b = normalize(cross(n,t));             // 1, 0, 0
	return transpose(mat3(b, n, t));
}

void main()
{
	const float Fcoef = 2.0 / log2(2000000.0 + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;

	// vec3 n = texture3D(oceanNormals,vec3(position.xz*0.0001*6.234,0.0002*time)).xyz * 0.35 + 
	// 	texture3D(oceanNormals,vec3(position.xz*0.0001*64.0,0.0003*time)).xyz * 0.35 +
	// 	texture3D(oceanNormals,vec3(position.xz*0.0001*512.0,0.0005*time)).xyz * 0.25; //must sum to 1
	// n.xz = (n.xz * 2.0 - vec2(1.0,1.0));
	// n = normalize(n);


	vec3 n = texture2D(waveTexture, position.xz*invTextureScale).yzw;
//		+ texture2D(waveTexture, position.xz*invTextureScale/32).yzw;
	n = normalize(n);
//	n = getTangentMatrix() * n;



//	vec4 color = vec4(0.1,0.4,0.9,1);
	///////////////LIGHT///////////////
	//vec4 treesTextureColor = texture2D(treesTexture, ttex_halfPixel + (position.xz-ttex_origin) * ttex_invScale); //tree shadows
	vec3 light = vec3(1,1,0.7) * (max(dot(n,normalize(sunDirection))/*0.5+0.5 /*- treesTextureColor.a*0.3*/, 0.0));
	//color.rgb *= light;

	///////////////GRID////////////////
	//vec2 px = position.xz/64.0 + 0.5;
	//vec2 fx = abs(fract(px) - 0.5)*10.0 ;
	//color.rgb *= max(     clamp(fx.x/fwidth(px.x*10.0),0.0,1.0) + clamp(fx.y/fwidth(px.y*10.0),0.0,1.0) - 1.0,     0.0);



	vec3 sunColor = vec3(1,1,.8);
	vec3 eyeDirection = normalize(eyePosition - position);

	float cosi = dot(n, eyeDirection);
	float cost = cos(asin(sin(acos(cosi)) / 1.333));
	float fresnel_r = -(cosi - 1.333 * cost) / (cosi + 1.333 * cost);
	float fresnel_t = 2.0 * cost / (cosi + 1.333 * cost);

	//see: https://research.microsoft.com/en-us/um/people/xtong/rtwave.pdf
	float c = max(dot(n, eyeDirection), 0);
	float g = sqrt( (1.333*1.333) + c*c - 1.0);
	float numer = c * (g + c) - 1.0;
	float denom = c * (g - c) + 1.0;
	float fresnel = 0.5 * ((g-c)*(g-c)) / ((g+c)*(g+c))
		* (1.0 + (numer*numer) / (denom*denom));
	fresnel *= .5;

	vec3 r = reflect(-normalize(eyePosition - position),n);
	r.y = abs(r.y);
	vec3 cReflect = textureCube(sky, r).rgb;// * vec3(0.1, .4, .7);


	vec4 color = vec4(cReflect,1);//vec4(0.64,0.89,1,0);//vec4(0.12, 0.26, 0.55, 0.0);//vec4(0.07, 0.35, 0.7, 0.0);
	color *= fresnel;
	color.a = fresnel;
	//color.a = 1.0 - fresnel * 0.5;
	//color.a = 1.0 - fresnel_t;

	color.rgb += 2.0*sunColor*vec3(pow(max(0.0, dot(reflect(-normalize(sunDirection), normalize(vec3(n.x,n.y,n.z))), normalize(eyeDirection))), 256.0));


//	float foam = clamp((0.5 + gHeight*0.05) * height, 0, 1);
//	color.rgb = mix(color.rgb, vec3(1), foam);
//	color.a = clamp(color.a + foam, 0, 1);

//	color.rgb = n;//vec3(0.1,0.4,0.9) * light * 0.25;

	//color = vec4(texture2D(waveTexture, position.xz*invTextureScale).xxx,1);
	
	color.a = 1.0 - (1.0 - color.a) * colorMult;

//	color.rgb = texture2D(waveTexture, fract(position.xz*invTextureScale)).xxx;
//	color.a = 1;
//	color.rgb = n.xxz;
		
	gl_FragColor = color; // vec4(vec3(height),1.0);// 
	///////////////DEPTH///////////////
	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	float depthSubtract = (1.0 - colorMult) * 0.001;
	gl_FragDepth = log2(flogz) * Fcoef_half - depthSubtract;
}
