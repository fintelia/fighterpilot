
varying vec2 texCoord;
varying float g;

uniform float gamma;

attribute vec2 Position;
attribute vec2 TexCoord;

void main()
{
	g = 1.0/gamma;
	texCoord = TexCoord;

	gl_Position = vec4(Position.x,Position.y,0.0,1.0);
}
