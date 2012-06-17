
varying vec2 texCoord;

uniform float sAspect;

attribute vec2 Position2;
attribute vec2 TexCoord;

void main()
{
	texCoord = TexCoord;
	gl_Position = vec4(Position2.x/sAspect*2.0-1.0,1.0-Position2.y*2.0,0.0,1.0);
}
