
varying vec2 position;
varying vec2 texCoord;

uniform float sAspect;

uniform vec4 viewConstraint = vec4(0,0,1,1);

attribute vec2 Position2;
attribute vec2 TexCoord;

void main()
{
	gl_Position = vec4((Position2.x/sAspect-viewConstraint.x)*2.0/viewConstraint.z-1.0, (1.0-Position2.y-viewConstraint.y)*2.0/viewConstraint.w-1.0,0.0,1.0);
	position = Position2;
	texCoord = TexCoord;
}
