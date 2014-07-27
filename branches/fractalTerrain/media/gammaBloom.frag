
uniform sampler2D tex;
varying float g;
varying vec2 texCoord;

void main()
{
	//    tone mapping
	//vec3 t = texture2D(tex,texCoord).rgb;
	//float L = 0.27*t.r + 0.67*t.g + 0.07*t.b;//max(t.r,max(t.g,t.b));
	//t *= (1.0 + L/1.1) / (L+1.0);
	//gl_FragColor = vec4(pow(t,vec3(g,g,g)),1.0);

	//vec3 blurColor = texture2DLod(tex, texCoord, 1.0).rgb;

	vec3 color = texture2D(tex, texCoord).rgb;
	//float i = color.r+color.g+color.b;
	float L = 0.27*color.r + 0.67*color.g + 0.07*color.b;
	color += L*L*L * 0.25;
	color = pow(color,vec3(g,g,g));
	gl_FragColor = vec4(color,1.0);

	
//	vec3 areaColor = texture2D(tex, texCoord,4.0).rgb;
//	vec3 color = texture2D(tex, texCoord).rgb;
//	vec3 colorBloom = texture2D(blurTex, texCoord).rgb;
//
//	float avgLuminance = dot(areaColor, vec3(0.3, 0.59, 0.11));
//	float sampleLuminance = dot(color, vec3(0.3, 0.59, 0.11));
//
//	vec3 colorSum = clamp(color,0.0,1.0) + max(colorBloom-1.0,0.0);
//	vec3 exposedColor = vec3(1.0) - exp(-colorSum * 1.5);
//
//	gl_FragColor = vec4(color,1);//vec4(exposedColor,1.0);
}
