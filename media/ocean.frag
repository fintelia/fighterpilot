
varying vec2 modelPosition;	//length will be at least 0.995 (assuming disk has 64 sides)
varying vec3 worldPosition;
varying vec2 texCoord;

varying vec3 sunDir, sunHalfVector;
varying vec3 eyeDirection;


uniform sampler3D oceanNormals;
uniform samplerCube sky;
uniform float time;
uniform vec3 sunColor;
uniform vec3 eyePos;

uniform sampler2D groundTexture;
uniform sampler2D depth;
//uniform sampler2D renderTexture;
uniform vec2 invGroundScale;
uniform vec2 halfPixelOffset;
uniform float groundHeightScale;
uniform float texSeaLevel;

//uniform vec2 invScreenDims;

//const float zNear = 1.0;
//const float zFar = 500000;

////float height(vec3 textureCoordinates)
////{
////	return 0.5*texture3D(oceanNormals,textureCoordinates).w + 0.3*texture3D(oceanNormals,vec3(textureCoordinates.xy*7.345, textureCoordinates.z)).w + 0.2*texture3D(oceanNormals,vec3(textureCoordinates.xy*20.345, textureCoordinates.z)).w;
////}

////vec3 normal(vec3 textureCoordinates, vec2 e)
////{
////	float h0 = height(textureCoordinates);
////	float hx = height(textureCoordinates+vec3(e.x,0,0));
////	float hz = height(textureCoordinates+vec3(0,e.y,0));

////	return normalize(vec3((h0-hx)/e.x, 1.0, (h0-hz)/e.y));
////}

////see: http://maverick.inria.fr/Publications/2006/BD06a/water06.pdf

//float interpolate(float v1, float v2, float t)
//{
//	return mix(v1,v2, (3.0*t*t - 2.0*t*t*t));
//}
//vec2 unitVec(float v)
//{
//	return vec2(cos(v * 6.2832),sin(v * 6.2832));
//}
//float perlin(vec3 texCoord)
//{

//	return 1.0 - 2.0*texture3D(oceanNormals,texCoord).r;

//	texCoord *= 128.0;
//	vec3 coord1 = vec3(floor(texCoord.xy),texCoord.z);
//	vec3 coord2 = coord1 + vec3(1,0,0);
//	vec3 coord3 = coord1 + vec3(0,1,0);
//	vec3 coord4 = coord1 + vec3(1,1,0);

//	vec2 v1 = (texCoord - coord1).xy;
//	vec2 v2 = (texCoord - coord2).xy;
//	vec2 v3 = (texCoord - coord3).xy;
//	vec2 v4 = (texCoord - coord4).xy;

//	//return dot(v3,texture3D(oceanNormals,coord1/128.0).xy);

//	vec2 g1 = unitVec(texture3D(oceanNormals,coord1/128.0).x);//normalize(texture3D(oceanNormals,coord1/128.0).xy*2.0 - 1.0);
//	vec2 g2 = unitVec(texture3D(oceanNormals,coord2/128.0).x);//normalize(texture3D(oceanNormals,coord2/128.0).xy*2.0 - 1.0);
//	vec2 g3 = unitVec(texture3D(oceanNormals,coord3/128.0).x);//normalize(texture3D(oceanNormals,coord3/128.0).xy*2.0 - 1.0);
//	vec2 g4 = unitVec(texture3D(oceanNormals,coord4/128.0).x);//normalize(texture3D(oceanNormals,coord4/128.0).xy*2.0 - 1.0);

//	//return interpolate(	interpolate(dot(v1,g1),	dot(v2,g2),	0.5/*fract(texCoord.x)*/),
//	//					interpolate(dot(v3,g3),	dot(v4,g4),	0.5/*fract(texCoord.x)*/),
//	//					0.5/*fract(texCoord.y)*/) * 0.5 + 0.5;
   




//	//return mix(	mix(dot(v1,normalize(texture3D(oceanNormals,coord1/128.0).xy)),
//	//				dot(v2,normalize(texture3D(oceanNormals,coord2/128.0).xy)),
//	//				fract(texCoord.x)),
//	//			mix(dot(v3,normalize(texture3D(oceanNormals,coord3/128.0).xy)),
//	//				dot(v4,normalize(texture3D(oceanNormals,coord4/128.0).xy)),
//	//				fract(texCoord.x)),
//	//			fract(texCoord.y));
//}

//float fractal(vec3 texCoord)
//{
//	return  abs(perlin(texCoord))/4 + 
//			abs(perlin(texCoord * vec3(2,2,1)))/(4) + 
//			abs(perlin(texCoord * vec3(4,4,1)))/(4) + 
//			abs(perlin(texCoord * vec3(8,8,1)))/(8) + 
//			abs(perlin(texCoord * vec3(16,16,1)))/(16);
//	//return 0.5 +	perlin(texCoord)*0.5 + 
//	//				perlin(texCoord * vec3(2,2,1))/(2*2) + 
//	//				perlin(texCoord * vec3(4,4,1))/(2*4) + 
//	//				perlin(texCoord * vec3(8,8,1))/(2*8) + 
//	//				perlin(texCoord * vec3(16,16,1))/(2*16);
//}
//vec3 normal(vec3 texCoord)
//{
//	float h1 = fractal(texCoord + vec3(-0.01,	0,		0));
//	float h2 = fractal(texCoord + vec3( 0.01,	0,		0));
//	float h3 = fractal(texCoord + vec3( 0,		-0.01,	0));
//	float h4 = fractal(texCoord + vec3( 0,		0.01,	0));

//	return normalize(vec3(h2-h1, 2.0, h4-h3));


//}
void main()
{
	vec3 n;
	vec3 color;
	float NdotL;


	n = texture3D(oceanNormals,vec3(texCoord.xy*6.234,0.0002*time)).xyz * 0.30 + 
		texture3D(oceanNormals,vec3(texCoord.xy * 64.0,0.0003*time)).xyz * 0.45 + 
		texture3D(oceanNormals,vec3(texCoord.xy * 512.0,0.0005*time)).xyz * 0.25; //must sum to 1
	n.xz = (n.xz * 2.0 - vec2(1.0,1.0));
	n = normalize(n);

//	n = normal(vec3(texCoord.xy,0.0002*time));

	NdotL = dot(n,normalize(sunDir));

//	float power = 2.0;
//	float bias = 0.2;
//	float fresnel = clamp(bias + (1.0-bias)*pow(1.0 - NdotL, power), 0.0,1.0); //0.0 = bias; 5.0 = power <--- these change the water reflections






	float cosAngle = dot(n, normalize(eyeDirection));
	float R0 = 0.2;
	float fresnel = clamp(R0 + (1.0-R0)*pow(1.0-cosAngle, 5.0), 0.0, 1.0);

	vec3 r = reflect(-normalize(eyeDirection),n);
	r.y = max(r.y,0.0);
	vec3 cReflect = textureCube(sky, r).rgb;


	//float bufferDepth = 2.0 * zNear * zFar / (zFar + zNear - texture2D(depth, invScreenDims * gl_FragCoord.xy).x * (zFar - zNear));
	//float fragDepth = 2.0 * zNear * zFar / (zFar + zNear - gl_FragCoord.z * (zFar - zNear));

	//float d = bufferDepth - fragDepth;
	float d2 = (texSeaLevel - texture2D(groundTexture, (worldPosition.xz)*invGroundScale - halfPixelOffset).a)*groundHeightScale;
	d2 = min(50.0, d2);

	//const vec3 darkColor = vec3(0.08, 0.24, 0.26);
	const vec3 lightColor = vec3(0.12, 0.26, 0.55);
	//vec3 groundColor = texture2D(renderTexture, invScreenDims * gl_FragCoord.xy).rgb;//vec3(0.47,0.333,0.165);
	vec3 darkColor = //mix(groundColor,// * exp(-vec3(d) / vec3(7.0, 30.0, 70.0)) +
	  sunColor * 0.8 * (exp(-vec3(d2) / vec3(7.0, 30.0, 70.0)));//,
	//1.0 - exp(-vec3(d) / vec3(7.0, 30.0, 70.0)));

;
	  //	  vec3(1.333) - vec3(1.0) / (vec3(0.75) + sunColor * (exp(-vec3(d2) / vec3(7.0, 30.0, 70.0))));

							   //texture2D(groundTexture, (worldPosition.xz - halfPixelOffset)*invGroundScale).a; //




//	vec3 cRefract = mix(groundColor, darkColor, pow()

//	color = mix(darkColor, lightColor, fresnel);
//	color = mix(color, cReflect, fresnel * 0.3 /* *shorelineAlpha*/);//   *   (0.6 + 0.4 * NdotL);

//	color = mix(darkColor, groundColor, vec3(pow(3, -depth/7), pow(3, -depth/30), pow(3, -depth/70)));
	color = mix(darkColor, cReflect, fresnel * 0.20 * clamp(0.02*(d2-20.0),0.0,1.0)) * (0.9 + 0.1 * NdotL);


//	color = mix(color, textureCube(sky, vec3(modelPosition.x,0,modelPosition.y)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection), 0.0, 1.0));

	//color += min(pow(max(dot(vec3(0,1,0),normalize(sunHalfVector)),0.0),8.0), 1.0);
	color += 2.0*/*fresnel*/sunColor*vec3(pow(max(0.0, dot(reflect(-normalize(sunDir), normalize(vec3(n.x,n.y*5.0,n.z))), normalize(eyeDirection))), 512.0));
	color += 2.0*/*fresnel*/sunColor*vec3(pow(max(0.0, dot(reflect(-normalize(sunDir), normalize(vec3(n.x,n.y*2.0,n.z))), normalize(eyeDirection))), 512.0));

	vec3 eyeDir = eyePos - worldPosition;
//	color = mix(color, vec3(0.5,0.5,0.5), clamp(1.0 - 100000000.0 / dot(eyeDirection,eyeDirection),0.0,0.75));

	gl_FragColor = /*vec4(vec3(fractal(vec3(texCoord.xy,0.0002*time))),1);*//*vec4(vec3(fractal(vec3(texCoord.xy,0.0002*time))),1);*/vec4(vec3(color),1);
}

