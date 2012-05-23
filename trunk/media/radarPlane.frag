
varying vec2 pos;

uniform vec2 center;  // (0,0) - (sAspect,1)
uniform float radius;
uniform vec4 color;

void main()
{
	if(dot(pos-center,pos-center) > radius*radius)
		discard;

	gl_FragColor = color;
}