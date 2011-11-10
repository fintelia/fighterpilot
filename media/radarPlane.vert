
varying vec2 pos;
varying vec4 color;

uniform float sAspect;

void main()
{
	color = gl_Color;

	gl_Position = vec4(gl_Vertex.x/sAspect*2.0-1.0,1.0-gl_Vertex.y*2.0,0.0,1.0);
	pos = gl_Vertex.xy;
}
