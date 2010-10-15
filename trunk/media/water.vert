varying vec2 texCoord;
varying vec3 pos;
varying float height;
uniform float heightRange;

void main()
{
	height=(gl_Vertex.y/heightRange+0.5);
	texCoord = gl_MultiTexCoord0.xy;
	gl_Position = ftransform();
	pos=gl_Vertex.xyz;
}
