
uniform sampler2D tex;
varying float g;

varying vec2 texCoord;

void main()
{
	//    tone mapping
	//vec3 t = texture2D(tex,texCoord).rgb;
	//t *= 1.0 / (1.0*t+1.0);
	//gl_FragColor = vec4(pow(t,vec3(g,g,g)),1.0);

	gl_FragColor = vec4(pow(texture2D(tex,texCoord).rgb,vec3(g,g,g)),1.0);
}