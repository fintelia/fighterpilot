
varying vec3 position, lightDir, halfVector;

uniform sampler2D bumpMap;
uniform sampler2D groundTex;
uniform sampler2D rock;
uniform sampler2D sand;
uniform float time;
uniform float seaLevel;
void main()
{
	vec4 waves,seaFloor;

	vec3 n = normalize(	texture2D(bumpMap,position.xz*4.0	+ vec2(-time*0.50*0.0000016	,time*0.87*0.0000016) ).rgb*0.1+
						texture2D(bumpMap,position.xz*1.0	+ vec2(time*0.70*0.000004	,time*0.71*0.000004)).rgb*0.2+
						texture2D(bumpMap,position.xz*0.25	+ vec2(-time*0.3*0.000001	,-time*0.95*0.000001)).rgb*0.3);

						
	float NdotL = dot(n,lightDir);
	
	if (NdotL > 0.0) 
		waves = vec4(0.1,0.2,0.3,1.0) + 0.8 * NdotL + vec4(300000000000.0,300000000000.0,300000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0);
		//waves = vec4(0.1 + 0.2*NdotL, 0.2 + 0.3*NdotL, 0.3 + 0.5*NdotL, 1.0);
	else
		waves = vec4(0.1,0.2,0.3,1.0);

	if(position.x>1.0 || position.x<0.0 || position.z>1.0 || position.z<0.0)
	{
		gl_FragColor = waves;
	}
	else
	{
		float depth = clamp((seaLevel-texture2D(groundTex,position.xz).a)*2.5,0.0,1.0);
		//if(depth < -0.2) discard;
		//depth = max(depth,0.0);

		vec3 normal = texture2D(groundTex,position.xz).xyz;
		normal.x = normal.x * 2.0 - 1.0;
		normal.z = normal.z * 2.0 - 1.0;
		normal = normalize(normal);
		float slope = acos(dot(vec3(0.0,1.0,0.0),normal));
		float r=0.0;
		float s1=0.90;//1.53
		float s2=0.50;//1.48
		if(slope>s1 		) 	r=1.0;
		else if(slope>s2 	)	r=(slope-s2)/(s1-s2);

		seaFloor = texture2D(rock,position.xz*4.0,depth*2.0) * (r)  + texture2D(sand,position.xz*4.0,depth*2.0) * (1.0-r);
		gl_FragColor = mix(seaFloor,    waves,    depth );//vec4(mix(      mix(texture2D(sand,position.xz*4.0,depth*2.0).rgb, texture2D(rock,position.xz*4.0,depth*2.0).rgb, r),    color.rgb,    depth ),1.0);
		//	gl_FragColor = vec4(depth,depth,depth,1.0);
	}
}
