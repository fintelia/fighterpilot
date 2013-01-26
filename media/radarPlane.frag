
varying vec2 position;
varying vec2 texCoord;

uniform vec2 center;  // (0,0) - (sAspect,1)
uniform float radius;
uniform vec4 color;

void main()
{
	//gl_FragColor = vec4(1,0,0,1);
	if(dot(position-center,position-center) > radius*radius)
		discard;

	vec2 tCoord = texCoord - vec2(0.5,0.5);
	float r = dot(tCoord,tCoord);//from center of square

	gl_FragColor = vec4(color.rgb, color.a * (1.0 - 4.0*r));
}