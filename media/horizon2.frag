
varying vec3 lightDir, eyeVec;
//varying float rad;
varying vec2 position;
varying vec2 texCoord;

uniform vec3 eyePos;

uniform sampler2D bumpMap;
uniform float time, seaLevel;
void main()
{
	vec3 n;
	float NdotL;
	vec4 color;
	
	//float alphaMult = dot(normalize(eyePos-pos),vec3(0.0,1.0,0.0));

	n =  normalize(		texture2D(bumpMap,texCoord.xy*10.0		+ vec2(-time*0.50*0.00001	, time*0.87*0.00001)).rgb*0.1 +
						texture2D(bumpMap,texCoord.xy*30.0		+ vec2(time*0.70*0.00001	, -time*0.71*0.00001)).rgb*0.2 +
						texture2D(bumpMap,texCoord.xy*80.0	+ vec2(-time*0.3*0.00004	, -time*0.95*0.00004)).rgb*0.3 +
						texture2D(bumpMap,texCoord.xy*320.0	+ vec2(-time*0.1*0.0001		, -time*0.995*0.0001)).rgb*0.3);

	NdotL = dot(n,lightDir);
	
	if (NdotL > 0.0) //vec4(10000,10000,10000,0.0)*pow(max(dot(n,normalize(halfVector)),0.0),32.0);//
		color = vec4(0.00630957, 0.0289912, 0.0707403,1.0) + 0.61 * NdotL; // + vec4(10,10,10,0) * pow(max(dot(n,normalize(halfVector)),0.0),60.0);
	else
		color = vec4(0.00630957, 0.0289912, 0.0289912,1.0);

	float r = length(position);
	color = mix(color,mix(vec4(0.0707403, 0.217638, 0.893289,1.0),vec4(0.172611, 0.531049, 0.893289,1.0),r),min(r+0.25,1.0));
	
//	if(gl_TexCoord[0].s<1.0 && gl_TexCoord[0].s>0.0 && gl_TexCoord[0].t<1.0 && gl_TexCoord[0].t>0.0)
//	{
//		float depth = clamp(((-seaLevel-texture2D(ground,gl_TexCoord[0].st).a))/-seaLevel,0.0,1.0);
//		color.a *= mix(depth,1.0,rad);
//		//color.a = 1.0 - (1.0 - color.a*mix(depth,1.0,rad)) * sqrt(alphaMult);
//		color.rgb = mix(color.rgb,vec3(1.0,1.0,1.0),(1.0-depth)*(1.0-depth)*0.6*(1.0-color.a));
//		//color.rgb = mix(vec3(1.0,1.0,1.0),color.rgb,color.a);
//	}
//	color.a += 0.03;

	//color.a = clamp(1.0/fwidth(r) * (0.99-r), 0.0, 1.0);	//0.99 might be too large since the water plane is a regular polygon, not a true circle.

	color = mix(vec4(0.5,0.5,0.5,0.0), color, clamp((2.0 - 5.0*r)*(2.0 - 3.0*r)*(2.0 - 3.0*r), 0.0, 1.0)); 

	gl_FragColor = color;
}