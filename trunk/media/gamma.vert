
varying vec2 texCoord;

void main()
{
	texCoord = gl_MultiTexCoord0.xy;

	gl_Position = vec4(gl_Vertex.x,gl_Vertex.y,0.0,1.0);
}
