
uniform sampler2D tex;
uniform vec4 color = vec4(1,1,1,1);
varying vec2 texCoord;

void main()
{
	vec4 outputColor = color * texture2D(tex, texCoord);
	if(outputColor.a < 0.001 || texCoord.x < 0.0 || texCoord.x > 1.0 || texCoord.y < 0.0 || texCoord.y > 1.0)
		discard;

	gl_FragColor = outputColor;
}
