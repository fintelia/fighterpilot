
varying vec2 texCoord;

attribute vec2 Position2;
attribute vec2 TexCoord;

void main()
{
	texCoord = TexCoord;
	gl_Position = vec4(Position2.x,Position2.y,0.0,1.0);
}
