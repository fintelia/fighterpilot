#version 330

in vec2 Position2;

out vec3 position;
out vec2 texCoord;

uniform float sideLength;

void main()
{
	position.xz = Position2 * sideLength * 0.5;
	texCoord = Position2 * 0.5 + 0.5;
	gl_Position = vec4(Position2.x,Position2.y,0.0,1.0);
}
