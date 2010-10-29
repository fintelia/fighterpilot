
varying vec3 position, lightDir, halfVector;
varying float height;

uniform sampler2D bumpMap;
uniform float time;

void main()
{
	vec4 color;

	vec3 n = normalize(	texture2D(bumpMap,position.xz*4.0	+ vec2(-time*0.50*0.0000016	,time*0.87*0.0000016) ).rgb*0.1+
						texture2D(bumpMap,position.xz*1.0	+ vec2(time*0.70*0.000004	,time*0.71*0.000004)).rgb*0.2+
						texture2D(bumpMap,position.xz*0.25	+ vec2(-time*0.3*0.000001	,-time*0.95*0.000001)).rgb*0.3);

						
	float NdotL = dot(n,lightDir);
	
	if (NdotL > 0.0) 
		color = vec4(0.1,0.2,0.3,1.0) + 0.8 * NdotL + vec4(300000000000.0,300000000000.0,300000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0);
	else
		color = vec4(0.1,0.2,0.3,1.0);

	gl_FragColor = color;
}
