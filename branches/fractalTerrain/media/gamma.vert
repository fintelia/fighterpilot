
varying vec2 texCoord;
varying float g;

uniform float gamma;

attribute vec2 Position2;
attribute vec2 TexCoord;

void main()
{
	g = 1.0/gamma;
	texCoord = TexCoord;

	gl_Position = vec4(Position2.x,Position2.y,0.0,1.0);
}
