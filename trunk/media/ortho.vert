
varying vec2 texCoord;
//varying vec4 color;

void main()
{
	texCoord = gl_MultiTexCoord0.xy;
	//color = gl_Color;
	gl_Position = vec4(gl_Vertex.xy,0.0,1.0);
}
