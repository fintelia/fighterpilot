//varying vec3 Normal;
//varying vec3 Light;
//varying vec3 HalfVector;
varying vec2 texCoord;
varying vec2 texCoord2;
varying vec3 normal;
//varying vec2 secondT;
uniform sampler2D myTexture[5];
//varying vec4 TexValues;
varying float height;
void main()
{
	float dist=gl_FragCoord.z/gl_FragCoord.w;		//if(dist>9000.0) discard;
	//Normal = normalize(Normal);

	//float Diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * max(dot(Normal, Light),0.0);

	//float Ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	//Ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;

	//float Specular = gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(max(dot(Normal,HalfVector),0.0), gl_FrontMaterial.shininess);

	//Normal = normalize(Normal);

	//vec4 Diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse * max(dot(Normal, Light),0.0);//

	//vec4 Ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;//
	//Ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;//
	//Ambient=vec4(0.4,0.4,0.4,1);
	//
	//float v= pow(max(dot(Normal,HalfVector),0.0), 50.0);// ;gl_FrontMaterial.shininess;
	//vec4 Specular =vec4(v,v,v,1.0);
	
	//vec2 t=vec2(texCoord.x*8.0-float(int(texCoord.x*8.0)),texCoord.y*8.0-float(int(texCoord.y*8.0)));

	//vec4 color = (texture2D(myTexture[0],texCoord)*(1.0-t.x	+1.0-t.y)
	//			+ texture2D(myTexture[1],texCoord)*(1.0-t.x	+t.y)
	//			+ texture2D(myTexture[2],texCoord)*(t.x		+1.0-t.y)
	//			+ texture2D(myTexture[3],texCoord)*(t.x		+t.y))/4.0;

	float slope = acos(dot(vec3(0.0,1.0,0.0),normalize(normal)));
	float rock=0.0;
	float s1=1.53;//1.55
	float s2=1.48;//1.4
	if(slope>s1 		) 	rock=1.0;
	else if(slope>s2 	)	rock=(slope-s2)/(s1-s2);

	vec4 TexValues;
	//if(height<1.0)
	//	TexValues = vec4(1.0,0.0,0.0,0.0);
	//else if(height<2.0)
	//	TexValues = vec4(1.0-(height-1.0),height-1.0,0.0,0.0);
	//if(height<0.2)
		//TexValues = vec4(0.0,1.0,0.0,0.0);
	//else if(height<0.4)
		//TexValues = vec4(0.0,1.0-(height-0.2)/0.2,(height-0.2)/0.2,0.0);
	//else //if(height<5.0)
		TexValues = vec4(0.0,0.0,1.0,0.0);
	//else if(height<6.0)
	//	TexValues = vec4(0.0,0.0,1.0-(height-5.0),height-5.0);
	//else if(height<7.0)
	//	TexValues = vec4(0.0,0.0,0.0,1.0);

	if(rock<TexValues[0]) rock=0.0;
	else rock-=TexValues[0];

	TexValues *= 1.0-rock;
	TexValues += vec4(0.0,0.0,0.0,rock);
	vec4 color = (texture2D(myTexture[0],texCoord		)*TexValues[0]
				+ texture2D(myTexture[1],texCoord		)*TexValues[1]
				+ texture2D(myTexture[2],texCoord*2.0	)*TexValues[2]
				+ texture2D(myTexture[3],texCoord*2.0	)*TexValues[3]);
	color = color*(1.0+0.5*TexValues[3]-texture2D(myTexture[4],texCoord*4.0).r*0.8*TexValues[3]);
	color = color*(1.2-texture2D(myTexture[4],texCoord*16.0).r*0.4);

	//vec3 c=min(Diffuse.rgb+Ambient.rgb,1.0);//(Ambient.rgb + Diffuse.rgb) + ;gl_FrontMaterial.specular *gl_LightSource[0].specular *
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float d=0.0001;
	float fogFactor = clamp(exp2( -d * d * z * z * 1.442695 ), 0.0, 1.0);
	fogFactor=1.0;
	color=mix(vec4(0.7,0.7,0.7,1.0), color, fogFactor);
	if(dist>8000.0) color.a*=1.0-(dist-8000.0)/1000.0;
	gl_FragColor = color;//vec4(color.rgb,1.0-fogFactor) + vec4(0.5,0.5,0.5,fogFactor);//min(color*vec4(c,1)+Specular*45.0,1.0);//*texture2D(myTexture, gl_TexCoord[0].st)*color.rgb
}