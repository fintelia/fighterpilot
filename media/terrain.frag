
varying vec3 position, lightDir, halfVector;

uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D groundTex;

void main()
{
	vec4 color, TexValues;
	if(position.x < 0.0 || position.x > 1.0 || position.y < 0.0 || position.y > 1.0 || position.z < 0.0 || position.z > 1.0) discard;
	//if(!gl_FrontFacing)
	//{
		//gl_FragColor = vec4(0.0,0.0,0.0,1.0);
		//return;
	//}

	if(position.y<0.1)
		TexValues=vec4(1.0,0.0,0.0,0.0);
	else if(position.y<0.15)
		TexValues=vec4(1.0-(position.y-0.1)/0.05,(position.y-0.1)/0.05,0.0,0.0);
	else if(position.y<0.5)
		TexValues=vec4(0.0,1.0,0.0,0.0);
	else if(position.y<0.6)
		TexValues=vec4(0.0,1.0-(position.y-0.5)/0.1,(position.y-0.5)/0.1,0.0);
	else if(position.y<0.75)
		TexValues=vec4(0.0,0.0,1.0,0.0);
	else if(position.y<0.85)
		TexValues=vec4(0.0,0.0,1.0-(position.y-0.75)/0.1,(position.y-0.75)/0.1);
	else//f(position.y<1.0)
		TexValues=vec4(0.0,0.0,0.0,1.0);

	color = (texture2D(sand,position.xz*4.0)*TexValues[0]
			+ texture2D(grass,position.xz*4.0)*TexValues[1]
			+ texture2D(rock,position.xz*8.0)*TexValues[2]
			+ texture2D(snow,position.xz*8.0)*TexValues[3]);

	vec3 normal = texture2D(groundTex,position.xz).xyz;
	normal.x = normal.x * 2.0 - 1.0;
	normal.z = normal.z * 2.0 - 1.0;
	normal = normalize(normal);
	float NdotL = dot(normal,lightDir);

	color *= (0.95 + clamp(NdotL*0.75,0.0,0.05));

	gl_FragColor = color ;
}