
uniform sampler2D tex;
varying vec2 texCoord;

void main()
{
	float pxSize = dFdx(texCoord.x);
	vec3 color = texture2D(tex, texCoord).rgb * 0.2270270270;
	color += texture2D(tex, texCoord + vec2(1.3846153846*pxSize),0.0).rgb * 0.3162162162;
	color += texture2D(tex, texCoord - vec2(1.3846153846*pxSize),0.0).rgb * 0.3162162162;	
	color += texture2D(tex, texCoord + vec2(3.2307692308*pxSize),0.0).rgb * 0.0702702703;
	color += texture2D(tex, texCoord - vec2(3.2307692308*pxSize),0.0).rgb * 0.0702702703;
	gl_FragColor = vec4(color,1.0);
}