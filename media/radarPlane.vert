
varying vec2 pos;
uniform float sAspect;

void main()
{

	gl_Position = vec4(gl_Vertex.x/sAspect*2.0-1.0,1.0-gl_Vertex.y*2.0,0.0,1.0);
	pos = gl_Vertex.xy;
}
