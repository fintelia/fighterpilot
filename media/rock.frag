
varying vec2 texCoord;
varying vec3 normal;
uniform sampler2D myTexture[3];

void main()
{
	float dist=gl_FragCoord.z/gl_FragCoord.w;

	float slope = acos(dot(vec3(0.0,1.0,0.0),normalize(normal)));
	float rock=0.0;
	float s1=1.93;//1.53
	float s2=1.68;//1.48
	if(slope>s1 		) 	rock=1.0;
	else if(slope>s2 	)	rock=(slope-s2)/(s1-s2);

	vec4 color = (texture2D(myTexture[0],texCoord		)*(1.0-rock)
				+ texture2D(myTexture[1],texCoord		)*rock);

	//color = color*(1.0+0.5*TexValues[3]-texture2D(myTexture[2],texCoord*4.0).r*0.8*TexValues[2]);
	
	color -= texture2D(myTexture[2],texCoord*4.0).r*0.1;


	gl_FragColor = color;//vec4(color.rgb,1.0-fogFactor) + vec4(0.5,0.5,0.5,fogFactor);//min(color*vec4(c,1)+Specular*45.0,1.0);//*texture2D(myTexture, gl_TexCoord[0].st)*color.rgb
}