
uniform vec4 color = vec4(1,1,1,1);
varying vec2 texCoord;

void main()
{
	float width = fwidth(texCoord.x); 
	float r = 2.0 * length( texCoord - vec2(0.5,0.5) ) + width;
	gl_FragColor = color * vec4(1.0,1.0,1.0,1.0 - clamp(0.5*abs(1.0 - r - width)/width,0.0,1.0));
}