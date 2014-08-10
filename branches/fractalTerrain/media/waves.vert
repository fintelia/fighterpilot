#version 330

out vec2 texCoord;

in vec2 Position2;
in vec2 TexCoord;

void main()
{
	texCoord = TexCoord;
	gl_Position = vec4(Position2.x,Position2.y,0.0,1.0);
}
