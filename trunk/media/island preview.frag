
varying vec3 position, lightDir, halfVector;
varying float h;

uniform float time;
uniform float heightScale;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;

void main()
{
	vec4 color;
	if(position.x < 0.0 || position.x > 1.0 /*|| position.y < 0.0 || position.y > 1.0*/ || position.z < 0.0 || position.z > 1.0)// || (position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5) > 0.25)
	 discard;
	//if(!gl_FrontFacing)
	//{
	//	gl_FragColor = vec4(0.0,0.0,0.0,1.0);
	//	return;
	//}

	vec3 normal = texture2D(groundTex,position.xz).xyz;
	normal.x = normal.x * 2.0 - 1.0;
	normal.z = normal.z * 2.0 - 1.0;
	normal.y /= heightScale;
	normal = normalize(normal);

	float dist=gl_FragCoord.z/gl_FragCoord.w;		//if(dist>9000.0) discard;
	float slope = acos(dot(vec3(0.0,1.0,0.0),normal));
	float r=0.0;
	float s1=1.10;//1.53
	float s2=0.80;//1.48
	if(slope>s1 		) 	r=1.0;
	else if(slope>s2 	)	r=(slope-s2)/(s1-s2);

	vec3 TexValues;
	if(h<0.2)		TexValues = vec3(0.0,1.0,0.0);
	else if(h<0.4)	TexValues = vec3(0.0,1.0-(h-0.2)/0.2,(h-0.2)/0.2);
	else				TexValues = vec3(0.0,0.0,1.0);

	//if(r<TexValues[0]) r=0.0;
	//else r-=TexValues[0];

	TexValues *= 1.0-r;
	TexValues += vec3(r,0.0,0.0);
	color = ( texture2D(rock,position.xz*4.0		)*TexValues[0]
			+ texture2D(sand,position.xz*4.0		)*TexValues[1]
			+ texture2D(grass,position.xz*4.0*2.0	)*TexValues[2]);
	color.rgb *= (1.0+0.5*TexValues[0]-texture2D(LCnoise,position.xz*4.0*4.0).r*0.8*TexValues[0]);
	color.rgb *= (1.2-texture2D(LCnoise,position.xz*4.0*16.0).r*0.4);
	//if(dist>80000.0) color.a*=1.0-(dist-80000.0)/10000.0;

	float NdotL = dot(normal,lightDir);

	//color.a *= clamp(1.0+position.y*2.0,1.0,0.0);
	color.a *= clamp(5.0-20.0*((position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5)), 0.0, 1.0);

	color = vec4(color.rgb*(NdotL*0.7+0.3),color.a);

	///////////////////////
	//if(position.y > 0.0)
	//{
		//float z = gl_FragCoord.z / gl_FragCoord.w;
		//float d=0.00001;
		//float fogFactor = clamp(exp2( -d * d * z * z * 1.442695 ), 0.0, 1.0);
		//color=mix(vec4(0.7,0.7,0.7,1.0), color, fogFactor);
	//}
	//////////////////

	gl_FragColor = color;//* (0.9 + clamp(NdotL*0.5,0.0,0.5));
}