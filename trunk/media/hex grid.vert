
varying vec2 texCoord;
varying float h;

uniform float minHeight;
uniform float maxHeight;
uniform float radius;

void main()
{
	texCoord = vec2(gl_MultiTexCoord0.x * radius / 30000.0 * 28.6 * (2.0 * 3.14159265), gl_MultiTexCoord0.y * (maxHeight - minHeight) / 30000.0 * 50.0);
	gl_Position = ftransform();

	h = 1.0 - (gl_Position.y-minHeight)/(maxHeight-minHeight);
}
