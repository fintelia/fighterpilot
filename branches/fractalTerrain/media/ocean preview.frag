
varying vec4 position;
varying vec3 lightDir, halfVector;

uniform sampler2D bumpMap;
//uniform sampler2D groundTex;
//uniform sampler2D rock;
//uniform sampler2D sand;
uniform float time;
//uniform float seaLevel;
void main()
{
	vec4 waves;

	vec3 n = normalize( texture2D(bumpMap,position.xz*0.0000775 + vec2(-time*0.50*0.000004 ,time*0.87*0.000004) ).rgb*0.1+
						texture2D(bumpMap,position.xz*0.0000194 + vec2(time*0.70*0.000001 ,time*0.71*0.000001)).rgb*0.2+
						texture2D(bumpMap,position.xz*0.00000484 + vec2(-time*0.3*0.00000025 ,-time*0.95*0.00000025)).rgb*0.3);

	float NdotL = dot(n,lightDir);

	if (NdotL > 0.0)
		waves = vec4(0.05,0.1 + 0.2*NdotL,0.1 + 0.4*NdotL,1.0)/* + vec4(300000000000.0,300000000000.0,300000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0)*/;
	else
		waves = vec4(0.1,0.2,0.3,1.0);

	gl_FragColor = waves;
}

