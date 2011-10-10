
varying vec2 texCoord;
varying vec3 normal;
varying vec4 color;
varying vec3 lightPosition;

uniform sampler2D tex;

void main()
{
	vec4 Color = vec4(color.rgb * texture2D(tex,texCoord).rgb,1.0);;
	//c.rgb *= max(dot(normal, lightPosition), 0.0);

	gl_FragColor = Color;
}