
varying vec3 position, lightDir, halfVector;
varying float h;

uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;
uniform sampler2D grass_normals;
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

	vec3 n = texture2D(groundTex,position.xz).xyz;
	n.x = n.x * 2.0 - 1.0;
	n.z = n.z * 2.0 - 1.0;
	n = normalize(n);
	n = vec3(0,1,0);
	vec3 t = normalize(cross(n, vec3(0,0,1)));
	vec3 b = cross(n, t);

	
	
	float dist=gl_FragCoord.z/gl_FragCoord.w;		//if(dist>9000.0) discard;
	float slope = acos(dot(vec3(0.0,1.0,0.0),n));
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
			+ texture2D(grass,position.xz*4.0*4.0	)*TexValues[2]);
	color.rgb *= 0.5 + 0.6*texture2D(LCnoise,position.xz*256.0).r;

	

	color.a *= clamp(5.0-20.0*((position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5)), 0.0, 1.0);
	

	///////////////////////
	//if(position.y > 0.0)
	//{
	//	float z = gl_FragCoord.z / gl_FragCoord.w;
	//	float d=0.0001;
	//	float fogFactor = clamp(exp2( -d * d * z * z * 1.442695 ), 0.0, 1.0);
	//	color.a=mix(vec4(0.7,0.7,0.7,1.0), color, fogFactor);
	//}
	//////////////////
	
	vec3 normal = normalize( mat3(n,t,-b)*mix(vec3(0,0,1), texture2D(grass_normals, position.xz*4.0*4.0).xyz*2.0 - 1.0, TexValues[2]));
	
	
	float NdotL = dot(normal,lightDir);

	float m = 1.0 + clamp(position.y/fwidth(position.y),-1.0,0.0)*(0.3-position.y*0.01);
	NdotL *= m;
	color.a *= m;
		
	color = vec4(color.rgb*(NdotL*0.5+0.5),color.a);
	gl_FragColor = color;
}