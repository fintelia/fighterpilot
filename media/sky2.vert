
attribute vec2 Position2;
attribute vec2 TexCoord;

varying vec2 position;

void main()
{
	position = TexCoord;
	gl_Position = vec4(Position2.xy, 1.0, 1.0);
}
