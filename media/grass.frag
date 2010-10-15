
varying vec3 position, lightDir, halfVector;
varying float seaLevel;

uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D waterBumpMap;

void main()
{
	vec4 color, TexValues;
	
	if(position.y<seaLevel)
	{
		float depth=(seaLevel-position.y)/seaLevel*2.5;
		vec3 n = normalize(	texture2D(waterBumpMap,position.xz*20.0	+ vec2(-time*0.50*0.0000016	, time*0.87*0.0000016)).rgb*0.1+
							texture2D(waterBumpMap,position.xz*5.0	+ vec2( time*0.70*0.000004	, time*0.71*0.000004)).rgb*0.2+
							texture2D(waterBumpMap,position.xz*1.5	+ vec2(-time*0.3*0.000001	,-time*0.95*0.000001)).rgb*0.3);
		float NdotL = dot(n,lightDir);

		color = vec4(0.1,0.2,0.3,1.0);
		if (NdotL > 0.0) 
			color = vec4(0.1,0.2,0.3,1.0) + 0.8 * NdotL + vec4(300000000000.0,300000000000.0,300000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0);
			
		color = mix(texture2D(sand,position.xz*25.0,depth*2.0), color, depth );
	}
	else
	{
		vec3 normal=vec3(0,1,0);

		float height=(position.y-seaLevel)/(1.0-seaLevel);
		float slope = acos(dot(vec3(0.0,1.0,0.0),normalize(normal)));
		slope=3.0-height*3.0;
		float r=0.0;
		float s1=1.53;//1.55
		float s2=1.48;//1.4
		if(slope>s1 		) 	r=1.0;
		else if(slope>s2 	)	r=(slope-s2)/(s1-s2);

		color = texture2D(grass,position.xz*16.0*3.0)*(1.0-r) + texture2D(rock,position.xz*16.0*3.0)*(r)*(1.0+0.5*r-texture2D(LCnoise,position.xz*16.0*4.0).r*0.8*r)*(1.2-texture2D(LCnoise,position.xz*16.0*16.0).r*0.4);

		//if(dist>8000.0) color.a*=1.0-(dist-8000.0)/1000.0;
	}


	gl_FragColor = color;
}