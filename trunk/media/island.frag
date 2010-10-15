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
	if(height<0.0)discard;

	vec4 color;
	float dist=gl_FragCoord.z/gl_FragCoord.w;		//if(dist>9000.0) discard;
	float slope = acos(dot(vec3(0.0,1.0,0.0),normalize(normal)));
	float rock=0.0;
	float s1=1.53;//1.55
	float s2=1.48;//1.4
	if(slope>s1 		) 	rock=1.0;
	else if(slope>s2 	)	rock=(slope-s2)/(s1-s2);

	vec4 TexValues;
	if(height<0.2)
		TexValues = vec4(0.0,1.0,0.0,0.0);
	else if(height<0.4)
		TexValues = vec4(0.0,1.0-(height-0.2)/0.2,(height-0.2)/0.2,0.0);
	else
		TexValues = vec4(0.0,0.0,1.0,0.0);

	if(rock<TexValues[0]) rock=0.0;
	else rock-=TexValues[0];

	TexValues *= 1.0-rock;
	TexValues += vec4(0.0,0.0,0.0,rock);
	color = (texture2D(myTexture[0],texCoord		)*TexValues[0]
			+ texture2D(myTexture[1],texCoord		)*TexValues[1]
			+ texture2D(myTexture[2],texCoord*2.0	)*TexValues[2]
			+ texture2D(myTexture[3],texCoord*2.0	)*TexValues[3]);
	color = color*(1.0+0.5*TexValues[3]-texture2D(myTexture[4],texCoord*4.0).r*0.8*TexValues[3]);
	color = color*(1.2-texture2D(myTexture[4],texCoord*16.0).r*0.4);

	//float z = gl_FragCoord.z / gl_FragCoord.w;
	//float d=0.0002;
	//float fogFactor = clamp(exp2( -d * d * z * z * 1.442695 ), 0.0, 1.0);
	//color=mix(vec4(0.7,0.7,0.7,1.0), color, fogFactor);

	if(dist>8000.0) color.a*=1.0-(dist-8000.0)/1000.0;

	gl_FragColor = color;
}