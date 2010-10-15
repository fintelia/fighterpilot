
varying vec2 texCoord;
varying vec3 normal;

void main()
{
	texCoord = gl_MultiTexCoord0.xy;
	normal = gl_Normal;

	gl_Position = ftransform();
}
