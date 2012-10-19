
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

	vec2 tSize = vec2(1.0/1280.0, 1.0/1024.0);

	vec3 blurColor = texture2DLod(tex, texCoord, 1.0).rgb;

	//vec3 blurColor =	(texture2D(tex, texCoord+vec2(tSize.x,tSize.y)*0.35).rgb + 
	//					texture2D(tex, texCoord+vec2(-tSize.x,tSize.y)*0.35).rgb + 
	//					texture2D(tex, texCoord+vec2(-tSize.x,-tSize.y)*0.35).rgb + 
	//					texture2D(tex, texCoord+vec2(tSize.x,-tSize.y)*0.35).rgb) * 0.25;

	float i = blurColor.r+blurColor.g+blurColor.b;
	vec3 color = texture2D(tex, texCoord).rgb + i*i*i * 0.05;
	color = pow(color,vec3(g,g,g));
	gl_FragColor = vec4(color,1.0);
}