
varying vec3 position, eyeDirection, lightDir, halfVector;
varying float h;

uniform float minHeight;
uniform float maxHeight;

uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;
uniform sampler2D grass_normals;
uniform sampler2D noiseTex;
uniform samplerCube sky;

void main()
{
	vec4 color;
	if(position.x < 0.0 || position.x > 1.0 || position.y < -45.0 /*|| position.y > 1.0*/ || position.z < 0.0 || position.z > 1.0)// || (position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5) > 0.25)
		discard;
	//if(!gl_FrontFacing)
	//{
	//	gl_FragColor = vec4(0.0,0.0,0.0,1.0);
	//	return;
	//}

	vec4 goundTexVal = texture2D(groundTex,position.xz);
	vec3 v = goundTexVal.xyz;
	vec3 n = normalize(vec3(v.x * 2.0 - 1.0, v.y, v.z * 2.0 - 1.0));
	//n = vec3(0,1,0);
	vec3 t = normalize(cross(n, vec3(0,0,1)));
	vec3 b = cross(n, t);

	
	
	//float dist=gl_FragCoord.z/gl_FragCoord.w;		//if(dist>9000.0) discard;
	//float slope = acos(dot(vec3(0.0,1.0,0.0),n));
	//float r=0.0;
	//float s1=1.30;//1.53;
	//float s2=0.90;//1.48;
	//if(slope>s1 		) 	r=1.0;
	//else if(slope>s2 	)	r=(slope-s2)/(s1-s2);

	//vec3 TexValues;
	//if(h<0.1)		TexValues = vec3(0.0,1.0,0.0);
	//else if(h<0.2)	TexValues = vec3(0.0,1.0-(h-0.1)/0.1,(h-0.1)/0.1);
	////else if(h<0.4)TexValues = vec3(r*(h-0.3)*10.0,0.0,1.0-r*(h-0.3)*10.0);
	////else			TexValues = vec3(r,0.0,1.0-r);
	//else			TexValues = vec3(0.0,0.0,1.0);

	//TexValues = mix(vec3(0.0,1.0,0.0), vec3(0.0,0.0,1.0), (h-0.1)/0.1);

	//if(r<TexValues[1]) r=0.0;
	//else r-=TexValues[1];

	//TexValues *= 1.0-r;
	//TexValues += vec3(r,0.0,0.0);
//	color = ( texture2D(rock,position.xz*32.0	)*TexValues[0]
//			+ texture2D(sand,position.xz*4.0		)*TexValues[1]
//			+ texture2D(grass,position.xz*4.0*4.0	)*TexValues[2]);
	
	float noiseVal = texture2D(noiseTex, position.xz*32.0).x + texture2D(LCnoise, position.xz*8.0).x;
	
	float g = pow(clamp(noiseVal + (h-0.05)/0.15 - 1.5, 0.0, 1.0), 2.0);
	color = mix(texture2D(sand,position.xz*4.0), texture2D(grass,position.xz*4.0*4.0), g);
	
	color.rgb *= 0.5 + 0.6*texture2D(LCnoise,position.xz*256.0).r;
	color.a *= clamp(5.0-20.0*((position.x-0.5)*(position.x-0.5)+(position.z-0.5)*(position.z-0.5)), 0.0, 1.0);
	
	vec3 normal = normalize( mat3(n,t,-b)*mix(vec3(0,0,1), texture2D(grass_normals, position.xz*4.0*4.0).xyz*2.0 - 1.0, g*g*g));

	
	float NdotL = dot(normal,lightDir);
	
	float height = position.y;//minHeight + goundTexVal.a * (maxHeight-minHeight);
	float m = 1.0 + clamp((height)/fwidth(height),-1.0,0.0)*(0.1-(height)*0.02);
	NdotL *= m;
	color.a *= m;

	//color = vec4(color.rgb*(NdotL*0.35+0.65),color.a * (1.0-clamp(0.000000002*dot(eyeDirection,eyeDirection),0.0,1.0)) );
	color = vec4(mix(color.rgb*(NdotL*0.35+0.65), textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a);
	gl_FragColor = color;
}