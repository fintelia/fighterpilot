
varying vec2 texCoord;
void main()
{

	texCoord = gl_MultiTexCoord0.xy;

	gl_Position = ftransform();//gl_ModelViewProjectionMatrix * gl_Vertex;
}
