
varying vec3 lightDir,halfVector;
varying float height, rad;

uniform sampler2D bumpMap, ground, tex;
uniform float time;

void main()
{
	vec3 n;
	float NdotL;
	vec4 color;
	
	n =  normalize(		texture2D(bumpMap,gl_TexCoord[0].st*4.0		+ vec2(-time*0.50*0.0000016	,time*0.87*0.0000016) ).rgb*0.1+
						texture2D(bumpMap,gl_TexCoord[0].st*1.0		+ vec2(time*0.70*0.000004	,time*0.71*0.000004)).rgb*0.2+
						texture2D(bumpMap,gl_TexCoord[0].st*0.25	+ vec2(-time*0.3*0.000001	,-time*0.95*0.000001)).rgb*0.3);


	
	NdotL = dot(n,lightDir);
	
	if (NdotL > 0.0) 
		color = vec4(0.1,0.2,0.3,1.0) + 0.8 * NdotL + vec4(300000000000.0,300000000000.0,300000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0);
		//color = vec4(0.1,0.2,0.3,1.0) + 0.7 * NdotL + vec4(3000000000000.0,3000000000000.0,3000000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0);
	else
		color = vec4(0.1,0.2,0.3,1.0);


	if(gl_TexCoord[0].s<1.0 && gl_TexCoord[0].s>0.0 && gl_TexCoord[0].t<1.0 && gl_TexCoord[0].t>0.0)
	{
		float depth = clamp((height-texture2D(ground,gl_TexCoord[0].st).r)*2.5,0.0,1.0);
		color = mix(texture2D(tex,gl_TexCoord[0].st*25.0,depth*2.0), color, depth );
	}
	gl_FragColor = mix(color,vec4(0.45,0.75,0.95,1.0),rad);
}
