
varying vec2 pos;
varying vec4 color;

uniform vec2 center;  // (0,0) - (sAspect,1)
uniform float radius;


void main()
{
	if(dot(pos-center,pos-center) > radius*radius)
		discard;

	gl_FragColor = color;
}