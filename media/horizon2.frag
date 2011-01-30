
varying vec3 lightDir,halfVector;
varying float rad;
varying vec3 pos;

uniform sampler2D bumpMap, ground, tex;
uniform float time, seaLevel;

void main()
{
	vec3 n;
	float NdotL;
	vec4 color;
	
	n =  normalize(		texture2D(bumpMap,gl_TexCoord[0].st*1.0		+ vec2(-time*0.50*0.000016	, time*0.87*0.000016)).rgb*0.1 +
						texture2D(bumpMap,gl_TexCoord[0].st*3.0		+ vec2(time*0.70*0.00004	, time*0.71*0.00004)).rgb*0.2 +
						texture2D(bumpMap,gl_TexCoord[0].st*8.0		+ vec2(-time*0.3*0.00001	, -time*0.95*0.00001)).rgb*0.3 +
						texture2D(bumpMap,gl_TexCoord[0].st*32.0	+ vec2(-time*0.1*0.0001		, -time*0.995*0.0001)).rgb*0.3);
	
	NdotL = dot(n,lightDir);
	
	if (NdotL > 0.0) //vec4(10000,10000,10000,0.0)*pow(max(dot(n,normalize(halfVector)),0.0),32.0);//
		color = vec4(0.1,0.2,0.3,1.0) + 0.8 * NdotL;// + vec4(300000000000.0,300000000000.0,300000000000.0,0.0) * pow(max(dot(n,normalize(halfVector)),0.0),100.0);
	else
		color = vec4(0.1,0.2,0.2,1.0);

	color = mix(color,mix(vec4(0.3,0.5,0.95,1.0),vec4(0.45,0.75,0.95,1.0),rad),min(rad+0.2,1.0));
	
	if(gl_TexCoord[0].s<1.0 && gl_TexCoord[0].s>0.0 && gl_TexCoord[0].t<1.0 && gl_TexCoord[0].t>0.0)
	{
		float depth = clamp((seaLevel-texture2D(ground,gl_TexCoord[0].st).a)*2.5+0.3,0.0,1.0);
		color = mix(texture2D(tex,gl_TexCoord[0].st*25.0,depth*2.0), color, depth );
	}
	gl_FragColor = color;
}