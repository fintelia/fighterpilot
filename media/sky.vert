#version 330

in vec2 Position2;
in vec2 TexCoord;

out vec2 position;

void main()
{
	position = TexCoord;
	gl_Position = vec4(Position2.xy, 1.0, 1.0);
}
