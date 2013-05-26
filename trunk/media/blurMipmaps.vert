
attribute vec2 TexCoord;
attribute vec2 Position2;
varying vec2 position;

void main()
{
	position = TexCoord;
	gl_Position = vec4(Position2,0,1);
}
