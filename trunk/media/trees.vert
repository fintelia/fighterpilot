
varying vec2 texCoord;
varying vec3 position;


attribute vec3 Position;
attribute vec2 TexCoord;

uniform mat4 cameraProjection;

void main()
{
	texCoord = TexCoord;
	position = Position;
	gl_Position = cameraProjection * vec4(Position,1.0);
}
