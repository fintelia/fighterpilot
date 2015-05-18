#version 330

attribute vec2 Position2;

void main()
{
	gl_Position = vec4(Position2.x,Position2.y,0.0,1.0);
}
