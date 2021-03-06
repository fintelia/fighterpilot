
varying vec2 texCoord;
varying float h;

uniform float minHeight;
uniform float maxHeight;
uniform float radius;

varying vec3 coord;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

attribute vec3 Position;
attribute vec2 TexCoord;

void main()
{
	texCoord = vec2(TexCoord.x * radius / 10000.0 * 28.6 * 6.0, TexCoord.y * (maxHeight - minHeight) / 10000.0 * 50.0 / (2.0 * 3.14159265) * 6.0);

	vec4 position = modelTransform * vec4(Position,1.0);
	gl_Position = cameraProjection * position;
	coord = gl_Position.xyz;
	h = 1.0 - (position.y-minHeight)/(maxHeight-minHeight);
}
