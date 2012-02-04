
//uniform vec3 color;
varying vec2 texCoord;

void main()
{
	float r = 2.0 * length( texCoord - vec2(0.5,0.5) ) + fwidth(texCoord.x);
	gl_FragColor = vec4(1.0,1.0,1.0,1.0 - clamp(0.5*abs(1.0 - r)/fwidth(texCoord.x),0.0,1.0));
}