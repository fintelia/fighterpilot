
varying vec3 position;
varying vec2 tCoord;
varying float flogz;

//uniform vec3 lightColors[4];
//uniform vec3 lightPositions[4];
//uniform float invLightStrengths[4];
uniform sampler2D LCnoise;
uniform sampler2D noiseTex;
uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D grassDetail;
uniform sampler2D groundTex;
uniform float slopeScale;
uniform vec3 eyePos;
uniform vec3 scale;
uniform vec3 sunDirection;
uniform vec3 eyePosition;


void main()
{
	float height = position.y;

//	if(position.y < -45.0)
//		discard;

	vec4 groundVal = texture2D(groundTex, tCoord);

	float slope = 2*max(abs(groundVal.g-0.5),abs(groundVal.b-0.5))*slopeScale;
	float slopeAngle = atan(slope);

	vec3 n = normalize(vec3((2*groundVal.g-1)*slopeScale, .01, (2*groundVal.b-1)*slopeScale));

	vec4 color = vec4(0,0,0,1);
	///////////GRASS AND SAND//////////
	float noiseVal = texture2D(noiseTex, position.xz*0.0025).x + texture2D(LCnoise, position.xz*0.000625).x;
	float gAmount = pow(clamp((noiseVal + (height-6.0)/20.0 - 1.5), 0.0, 1.0), 0.5);
	color = mix(texture2D(sand,position.xz*0.0003), mix(vec4(1,1,1,1), texture2D(grassDetail, position.xz*0.042) * 2.0,clamp(gAmount,0.0,1.0)) * texture2D(grass,position.xz*0.00125 *4.0)/* / texture2D(grassDetail, position.xz*0.0532,3.0)*/, gAmount);

	////////////TREE CANOPY////////////
	color.rgb *= 1.0 - 0.6 * clamp((height - 37.0)*0.2, 0.0, 1.0) * max(clamp((148.0-height)*0.2,0.0,1.0),clamp((slopeAngle - 0.1) / 0.1, 0.0, 1.0));
	//////////////ROCK/////////////////
	float rAmount = /*clamp(height * 0.02, 0.0, 1.0) */ clamp((slopeAngle - 0.40) / 0.06, 0.0,1.0);
	color.rgb = mix(color.rgb, vec3(0.15,0.14,0.13)*2.0/*0.5*texture2D(rock,position.xz*0.00125 * 16.0).rgb*/, rAmount); //vec4(0.35,0.25,0.20,1.0)
	//vec4 fractalColor = texture2D(fractalNormals, position.xz*0.0023);
	//color.rgb *= mix(1.0, 0.5+0.5*fractalColor.a, rAmount);




/*****************************************************************/
	float fractalNoise = texture2D(noiseTex, position.xz*0.0025).x + texture2D(LCnoise, position.xz*0.000625).x;
	if(slopeAngle > 0.3) {
		color.rgb = vec3(.4, .4, .39);
	}else if(position.y < 5.0 + fractalNoise) {
		color.rgb = texture2D(sand,position.xz*0.0003).rgb*1.5;
	}else{
		color.rgb = vec3(0, .65, .11);
	}
/*****************************************************************/



	vec3 positionToEye = eyePos - position;
	//vec3 positionToEye_tangentSpace = mat3(t,n,b)*positionToEye;
	//vec2 offset = -normalize(positionToEye_tangentSpace.xz) * (1.0 - fractalColor.a);

	//fractalColor = texture2D(fractalNormals, (position.xz+offset)*0.0023 * 0.5);
	//n = normalize( mat3(t,n,b)*mix(vec3(0,1,0), normalize(fractalColor.xzy*2.0 - 1.0), rAmount));

	//color = vec4(.1,.7,.1,1);
	
	///////////////LIGHT///////////////
	//vec4 treesTextureColor = texture2D(treesTexture, ttex_halfPixel + (position.xz-ttex_origin) * ttex_invScale); //tree shadows
	vec3 light =vec3(1,1,0.7) * (max(dot(n,normalize(sunDirection))*0.5+0.5 /*- treesTextureColor.a*0.3*/, 0.0));

//	light += 0.7*pow(max(dot(reflect(-sunDirection,n), normalize(eyePosition-position)),0.0),20.0);
	//for(int i=0;i<4;i++)
	//{
	//	vec3 lightVec = position - lightPositions[i];
	//	light += lightColors[i] * clamp(0.5 - length(lightVec) * invLightStrengths[i], 0.0,0.5);
	//}
//	light = clamp(light, 0.0, 1.0);
	color.rgb *= light;
	////////////WATER EFFECT///////////
	//see: http://www-evasion.imag.fr/~Fabrice.Neyret/images/fluids-nuages/waves/Jonathan/articlesCG/rendering-natural-waters-00.pdf
	float c = 0.000;
	float depth = max(-height/max(abs(normalize(eyePosition-position).y),1e-6),0);
	color.rgb *= exp(-depth / vec3(7.0,30.0,70.0));
	color.rgb += vec3(0.00, 0.2, 0.3) * (vec3(1.0) - exp(vec3(-c*depth) + 3*min(height,0) / vec3(7.0,30.0,70.0)));
	//float waterAlpha = clamp(1.0 - depth*0.03, 0.0,1.0);

	//light *= waterAlpha;
	//color.a = clamp(color.a*waterAlpha, 0.0, 1.0);

	////////////////FOG////////////////
	//vec3 eyeDirection = position.xyz-eyePos;
	//color = vec4(mix(color.rgb*max(light,0.0), textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*0.00001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a);

	///////////////GRID////////////////
	//vec2 px = position.xz/100.0 + 0.5;
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

	gl_FragColor = vec4((groundVal.zzy),1);//vec4(color.rgb,1.0); // vec4(vec3(height),1.0);// 

	///////////////DEPTH///////////////
	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	const float Fcoef = 2.0 / log2(500000.0 + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;
	gl_FragDepth = log2(flogz) * Fcoef_half;
}
