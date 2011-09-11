
varying vec2 texCoord;
varying vec3 normal;
varying vec4 color;
varying vec3 lightPosition;

uniform sampler2D tex;

void main()
{
	//color.rgb *= max(dot(normal, lightPosition), 0.0);
	gl_FragColor = vec4(color.rgb * texture2D(tex,texCoord).rgb,1.0);
//	gl_FragColor = vec4(texCoord.rr,1.0,1.0);
}