
varying vec2 texCoord;
varying vec2 texCoord2;
varying vec3 normal;

uniform float heightRange;

uniform float minHeight;
uniform float maxHeight;

varying float height;
void main()
{
	height=(gl_Vertex.y-minHeight)/(maxHeight-minHeight);
	//if(gl_Vertex.y<0.0)gl_Vertex.y=0.0;

	texCoord = gl_MultiTexCoord0.xy;
	texCoord2 = gl_MultiTexCoord4.xy;
	normal = gl_Normal;

	gl_Position = ftransform();
}
