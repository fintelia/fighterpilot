
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

	vec3 color = texture2D(tex, texCoord).rgb;
	color = pow(color,vec3(g,g,g));

    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
	gl_FragColor = vec4(color, luminance);
}
