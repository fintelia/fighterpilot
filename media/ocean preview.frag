
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

	vec3 n = normalize( texture2D(bumpMap,position.xz*1.0 + vec2(-time*0.50*0.000004 ,time*0.87*0.000004) ).rgb*0.1+
	texture2D(bumpMap,position.xz*0.25 + vec2(time*0.70*0.000001 ,time*0.71*0.000001)).rgb*0.2+
	texture2D(bumpMap,position.xz*0.0625 + vec2(-time*0.3*0.00000025 ,-time*0.95*0.00000025)).rgb*0.3);


	float NdotL = dot(n,lightDir);

	if (NdotL > 0.0)
	waves = vec4(0.05,0.1 + 0.2*NdotL,0.1 + 0.4*NdotL,1.0)/* + vec4(300000000000.0,300000000000.0,300000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0)*/;
	else
	waves = vec4(0.1,0.2,0.3,1.0);

	//if(position.x>1.0 || position.x<0.0 || position.z>1.0 || position.z<0.0)
	//{
		gl_FragColor = waves;
	//}
	//else
	//{
	//	float depth = clamp((seaLevel-texture2D(groundTex,position.xz).a)*5.5,0.0,1.0);
	//	gl_FragColor = vec4(waves.rgb,depth);
	//}
}

