
varying vec2 texCoord;
uniform float sAspect;

void main()
{
	texCoord = gl_MultiTexCoord0.xy;

	gl_Position = vec4(gl_Vertex.x/sAspect*2.0-1.0,1.0-gl_Vertex.y*2.0,0.0,1.0);
}
