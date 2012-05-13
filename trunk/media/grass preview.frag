
varying vec3 position;
varying vec3 lightDir, halfVector;

uniform float time;
uniform float heightScale;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;

uniform vec3 invScale;

void main()
{
	vec4 color;

	vec3 normal = texture2D(groundTex,position.xz * invScale.xz).xyz;
	normal.x = normal.x * 2.0 - 1.0;
	normal.z = normal.z * 2.0 - 1.0;
	normal.y /= heightScale;
	normal = normalize(normal);

	float NdotL = dot(normal, normalize(lightDir));

	float slope = acos(dot(vec3(0.0,1.0,0.0),normal));
	float r=0.0;
	float s1=1.10;//1.53
	float s2=0.80;//1.48
	if(slope>s1 		) 	r=1.0;
	else if(slope>s2 	)	r=(slope-s2)/(s1-s2);

	vec3 TexValues;
	//if(position.y<10.0)		TexValues = vec3(0.0,1.0,0.0);
	//else if(position.y<0.4)	TexValues = vec3(0.0,1.0-(position.y-0.2)/0.2,(position.y-0.2)/0.2);
	//else					TexValues = vec3(0.0,0.0,1.0);
	
	//if(r<TexValues[0]) r=0.0;
	//else r-=TexValues[0];

//	if(slope < 0.3) TexValues = vec3(0.0,1.0-(slope)/0.3*h,(slope)/0.3*h);
//	else TexValues = vec3(0.0,0.0,1.0);
	if(position.y<25.0)			TexValues = vec3(0.0,1.0,0.0);
	else if(position.y<50.0)	TexValues = vec3(0.0,1.0-(position.y-25.0)/25.0,(position.y-25.0)/25.0);
	else						TexValues = vec3(0.0,0.0,1.0);

	/*TexValues *= 1.0-r;
	TexValues += vec3(r,0.0,0.0);*/
	color = ( texture2D(rock,position.xz*0.0003		)*TexValues[0]
			+ texture2D(sand,position.xz*0.0003		)*TexValues[1]
			+ texture2D(grass,position.xz*0.0006	)*TexValues[2]);

	color.rgb *= 0.4 + 0.6 * NdotL;
	gl_FragColor = color;
}