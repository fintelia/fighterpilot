
uniform sampler2D tex;
uniform float gamma;

varying vec2 texCoord;

void main()
{
	gl_FragColor = vec4(pow(texture2D(tex,texCoord).rgb,1.0/gamma),1.0);
}