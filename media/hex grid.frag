
varying vec2 texCoord;
uniform vec3 color;


uniform sampler2D tex;
varying float h;

void main()
{
	vec4 tColor = texture2D(tex,texCoord);
	gl_FragColor = vec4(color.rgb, max(h*0.3, tColor.a*(h*0.3+0.1)));

}