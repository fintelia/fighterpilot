#version 330

in vec2 Position2;
in vec2 TexCoord;

out vec2 texCoord;

void main()
{
	texCoord = TexCoord;
	gl_Position = vec4(Position2.x,Position2.y,0.0,1.0);
}
