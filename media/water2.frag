
varying vec3 lightDir,halfVector;
varying float height;

uniform sampler2D bumpMap, ground, tex;
uniform float time;

void main()
{
	vec3 n;
	float NdotL;
	vec4 color;
	n = normalize(	//texture2D(bumpMap,gl_TexCoord[0].st*64.0	+ vec2(time*0.3		,-time*0.95) * 0.000256).rgb*0.1+
					//texture2D(bumpMap,gl_TexCoord[0].st*16.0	+ vec2(-time*0.30	,-time*0.95)  * 0.000064).rgb*0.2+
					texture2D(bumpMap,gl_TexCoord[0].st*4.0		+ vec2(-time*0.50*0.0000016	,time*0.87*0.0000016) ).rgb*0.1+
					texture2D(bumpMap,gl_TexCoord[0].st*1.0		+ vec2(time*0.70*0.000004	,time*0.71*0.000004)).rgb*0.2+
					texture2D(bumpMap,gl_TexCoord[0].st*0.25	+ vec2(-time*0.3*0.000001	,-time*0.95*0.000001)).rgb*0.3);
	
	NdotL = dot(n,lightDir);
	
	if (NdotL > 0.0) 
		color = vec4(0.1,0.2,0.3,1.0) + 0.7 * NdotL + vec4(3000000000000.0,3000000000000.0,3000000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0);
	else
		color = vec4(0.1,0.2,0.3,1.0);

	gl_FragColor = color;//mix(texture2DLod(tex,gl_TexCoord[0].st*50.0,3.0), color, clamp((height-texture2DLod(ground,clamp(gl_TexCoord[0].st,vec2(0.0,0.0),vec2(1.0,1.0)),1.0).r)*3.0,0.0,1.0) );
}
