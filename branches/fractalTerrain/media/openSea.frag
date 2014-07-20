
//varying vec4 diffuse,ambient;
varying vec3 lightDir,halfVector;
uniform sampler2D bumpMap;
uniform float time;
void main()
{
	vec3 n,halfV;
	float NdotL,NdotHV;
		
	/* The ambient term will always be present */
	vec4 color = vec4(0.1,0.2,0.3,1.0);

	/* a fragment shader can't write a varying variable, hence we need
	a new variable to store the normalized interpolated normal */
	n = normalize(	texture2D(bumpMap,gl_TexCoord[0].st*100.0+vec2(-time*0.3,time*0.95) *0.0001).rgb*0.04+
					texture2D(bumpMap,gl_TexCoord[0].st*30.0+vec2(time*0.3,-time*0.95)  *0.00008).rgb*0.1+
					texture2D(bumpMap,gl_TexCoord[0].st*10.0+vec2(-time*0.30,-time*0.95)*0.00004).rgb*0.2+
					texture2D(bumpMap,gl_TexCoord[0].st*2.0+vec2(time*0.50,-time*0.87) *0.00002).rgb*0.3+
					texture2D(bumpMap,gl_TexCoord[0].st*1.0+vec2(time*0.70,time*0.71)  *0.00001).rgb*0.4);
	
	/* compute the dot product between normal and ldir */
	NdotL = max(dot(n,lightDir),0.0);
	
	if (NdotL > 0.0) {
		color += 0.7 * NdotL;
		halfV = normalize(halfVector);
		NdotHV = max(dot(n,halfV),0.0);
		color += vec4(1.0,1.0,1.0,0.0) * 2000000000000.0 * pow(NdotHV,100.0);
	}
	
	gl_FragColor = color;

}
