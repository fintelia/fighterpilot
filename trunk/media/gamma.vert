
varying vec2 texCoord;
varying float g;

uniform float gamma;

void main()
{
	g = 1.0/gamma;
	texCoord = gl_MultiTexCoord0.xy;

	gl_Position = vec4(gl_Vertex.x,gl_Vertex.y,0.0,1.0);
}
