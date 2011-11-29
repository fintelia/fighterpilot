
varying vec2 texCoord;
//varying vec3 normal;
//varying vec4 color;
//varying vec3 lightPosition;
//varying vec4 position;

uniform sampler2D tex;

void main()
{
	vec4 Color = vec4(texture2D(tex,texCoord).rgb,1.0);
	gl_FragColor = Color;
}