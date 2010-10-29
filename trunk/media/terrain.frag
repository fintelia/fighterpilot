
varying vec3 position, lightDir, halfVector;

uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;

void main()
{
	vec4 color, TexValues;
	if(position.x < 0.0 || position.x > 1.0 || position.y < 0.0 || position.y > 1.0 || position.z < 0.0 || position.z > 1.0) discard;
	//if(position.y<seaLevel)
	//{
		//float depth=(seaLevel-position.y)/seaLevel*3.5;
		//vec3 n = normalize(	texture2D(waterBumpMap,position.xz*20.0	+ vec2(-time*0.50*0.0000016	, time*0.87*0.0000016)).rgb*0.1+
							//texture2D(waterBumpMap,position.xz*3.0	+ vec2( time*0.70*0.000004	, time*0.71*0.000004)).rgb*0.2+
							//texture2D(waterBumpMap,position.xz*0.5	+ vec2(-time*0.3*0.000001	,-time*0.95*0.000001)).rgb*0.3);
		//float NdotL = dot(n,lightDir);

		//color = vec4(0.1,0.2,0.3,1.0);
		//if (NdotL > 0.0) 
			//color = vec4(0.1,0.2,0.3,1.0) + 0.8 * NdotL + vec4(300000000000.0,300000000000.0,300000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0);
			//
		//color = mix(texture2D(sand,position.xz*20.0), color, depth);//,depth*40.0
	//}
	//else
	//{
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
		color = (texture2D(sand,position.xz*20.0)*TexValues[0]
				+ texture2D(grass,position.xz*60.0)*TexValues[1]
				+ texture2D(rock,position.xz*60.0)*TexValues[2]
				+ texture2D(snow,position.xz*20.0)*TexValues[3]);
	//}


	gl_FragColor = color;
}