
centroid varying vec3 position;
varying vec4 groundVal;
varying float flogz;

//uniform vec3 lightColors[4];
//uniform vec3 lightPositions[4];
//uniform float invLightStrengths[4];
uniform sampler3D oceanNormals;
uniform sampler2D colorBuffer;
uniform sampler2D depthBuffer;
uniform sampler2D waves;
uniform samplerCube sky;

uniform vec2 invScreenDims;
uniform float slopeScale;
uniform vec3 eyePos;
uniform vec3 scale;
uniform vec3 sunDirection;
uniform vec3 eyePosition;
uniform float time;

const int NUM_WAVES = 16;
uniform float amplitudes[NUM_WAVES];
uniform float frequencies[NUM_WAVES];
uniform float waveSpeeds[NUM_WAVES];
uniform vec2 waveDirections[NUM_WAVES];

void main()
{
	const float Fcoef = 2.0 / log2(500000.0 + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;
	float logBufferDepth = texture2D(depthBuffer, 
									 invScreenDims * gl_FragCoord.xy).x;
	float bufferDepth = exp2(logBufferDepth / Fcoef_half);
	float depth = bufferDepth - flogz;

	vec3 bufferColor = texture2D(colorBuffer, 
									 invScreenDims * gl_FragCoord.xy).rgb;

	float height = position.y;
	float slope =0;// 2*max(abs(groundVal.g-0.5),abs(groundVal.b-0.5))*slopeScale;
	float slopeAngle = atan(slope);

	/*vec3 n = vec3(0,0,0);//normalize(vec3((2*groundVal.g-1)*slopeScale, 1, (2*groundVal.b-1)*slopeScale));
	
	//float A[4] = float[4](6.1, 5.7, 4.5, 3.3);
	//float f[4] = float[4](0.005, 0.015, 0.028, 0.0121);
	//vec2 D[4] = vec2[4](vec2(1,0), vec2(0.6,-0.8), vec2(0,1), vec2(-0.8,-0.6));
	for(int i=0; i < NUM_WAVES; i++){
		vec4 w = texture(waves, vec2((0.001*waveSpeeds[i]*time + frequencies[i]*dot(position.xz,waveDirections[i])), 1));
		float slope = (w.y * 2.0 - 1.0) * 128 + (w.z * 2.0 - 1.0);
//		float slope = sin((frequencies[i] * dot(position.xz,waveDirections[i])
//						   + 0.001*waveSpeeds[i]*time) / (2*3.141592));
		vec3 norm;
		n.xz += waveDirections[i] * slope * frequencies[i] * amplitudes[i];
		n.y += 1.0;
	}

	n = normalize(n);*/

	vec3 n = texture3D(oceanNormals,vec3(position.xz*0.0001*6.234,0.0002*time)).xyz * 0.35 + 
		texture3D(oceanNormals,vec3(position.xz*0.0001*64.0,0.0003*time)).xyz * 0.35 +
		texture3D(oceanNormals,vec3(position.xz*0.0001*512.0,0.0005*time)).xyz * 0.25; //must sum to 1
	n.xz = (n.xz * 2.0 - vec2(1.0,1.0));
	n = normalize(n);

	

	vec4 color = vec4(0.1,0.4,0.9,1);
	///////////GRASS AND SAND//////////

	///////////////LIGHT///////////////
	//vec4 treesTextureColor = texture2D(treesTexture, ttex_halfPixel + (position.xz-ttex_origin) * ttex_invScale); //tree shadows
	vec3 light = vec3(1,1,0.7) * (max(dot(n,normalize(sunDirection))/*0.5+0.5 /*- treesTextureColor.a*0.3*/, 0.0));

	//light =+ 0.7*pow(max(dot(reflect(-sunDirection,n), normalize(eyePosition-position)),0.0),20.0);
	//for(int i=0;i<4;i++)
	//{
	//	vec3 lightVec = position - lightPositions[i];
	//	light += lightColors[i] * clamp(0.5 - length(lightVec) * invLightStrengths[i], 0.0,0.5);
	//}
//	light = clamp(light, 0.0, 1.0);
	color.rgb *= light;
	////////////WATER EFFECT///////////
	//float waterAlpha = clamp(1.0 + height*0.03, 0.0,1.0);

	//light *= waterAlpha;
	//color.a = clamp(color.a*waterAlpha, 0.0, 1.0);

	////////////////FOG////////////////
	//vec3 eyeDirection = position.xyz-eyePos;
	//color = vec4(mix(color.rgb*max(light,0.0), textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*0.00001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a);

	///////////////GRID////////////////
	//vec2 px = position.xz/64.0 + 0.5;
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
	//color.rgb = vec3(pow(max(dot(reflect(-sunDirection,n), normalize(eyePosition-position)),0.0),1.0));

//	if(depth < 0)
//		discard;

	//color.rgb =// bufferColor * exp(-depth/vec3(7.0, 30.0, 70.0)) + 
	//	0.8 * exp(-depth/vec3(7.0, 30.0, 70.0));

	//color.rgb = mix(color.rgb, bufferColor, clamp(1.0-depth*0.025,0,1));

//	color.rgb = vec3(fract(0.0005*time + f[0]*dot(position.xz,D[0])));//vec3(n);
//	color.rgb = texture(waves, vec2((0.0001*time + f[0]*dot(position.xz,D[0])), 0)).yyy;




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

	vec3 r = reflect(-normalize(eyePosition - position),n);
	r.y = abs(r.y);
	vec3 cReflect = textureCube(sky, r).rgb * vec3(0.1, .4, .7);

	color = vec4(cReflect,1);//vec4(0.64,0.89,1,0);//vec4(0.12, 0.26, 0.55, 0.0);//vec4(0.07, 0.35, 0.7, 0.0);
	color *= fresnel;
	color.a = fresnel;
	//color.a = 1.0 - fresnel * 0.5;
	//color.a = 1.0 - fresnel_t;

	color.rgb += 2.0*sunColor*vec3(pow(max(0.0, dot(reflect(-normalize(sunDirection), normalize(vec3(n.x,n.y,n.z))), normalize(eyeDirection))), 256.0));

	gl_FragColor = color; // vec4(vec3(height),1.0);// 
	///////////////DEPTH///////////////
	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	gl_FragDepth = log2(flogz) * Fcoef_half;
}
