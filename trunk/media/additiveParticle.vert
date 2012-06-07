
varying vec2 texCoord;
varying vec4 color;

uniform mat4 cameraProjection;

attribute vec3 Position;
attribute vec2 TexCoord;
attribute vec4 Color;

void main()
{
	texCoord = TexCoord;
	color = Color;

	gl_Position = cameraProjection * vec4(Position,1.0);
}
