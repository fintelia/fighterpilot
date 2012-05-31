
varying vec2 texCoord;
uniform float sAspect;

attribute vec2 Position;
attribute vec2 TexCoord;

void main()
{
	texCoord = TexCoord;
	gl_Position = vec4(Position.x/sAspect*2.0-1.0,1.0-Position.y*2.0,0.0,1.0);
}
