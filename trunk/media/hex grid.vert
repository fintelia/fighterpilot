
varying vec2 texCoord;
varying float h;

uniform float minHeight;
uniform float maxHeight;
uniform float radius;

varying vec3 coord;

void main()
{
	texCoord = vec2(gl_MultiTexCoord0.x * radius / 10000.0 * 28.6 * 6.0, gl_MultiTexCoord0.y * (maxHeight - minHeight) / 10000.0 * 50.0 / (2.0 * 3.14159265) * 6.0);
	gl_Position = ftransform();
	coord = gl_Position.xyz;
	h = 1.0 - (gl_Position.y-minHeight)/(maxHeight-minHeight);
}
