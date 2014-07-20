
attribute vec2 Position2;
attribute vec2 TexCoord;

varying vec2 position;

void main()
{
	position = TexCoord;
	gl_Position = vec4(Position2.xy, 0.0, 1.0);//vec4((Position2.x/sAspect-viewConstraint.x)*2.0/viewConstraint.z-1.0, (1.0-Position2.y-viewConstraint.y)*2.0/viewConstraint.w-1.0,0.0,1.0);
}
