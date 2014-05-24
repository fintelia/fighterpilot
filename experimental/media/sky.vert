
varying vec2 texCoord;
varying vec3 position;

varying vec4 color;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

attribute vec3 Position;
attribute vec2 TexCoord;

void main()
{
	position = Position;
	texCoord = TexCoord;

	gl_Position = cameraProjection * modelTransform * vec4(Position,1.0);
	gl_Position.z = clamp(gl_Position.z,-0.999,0.999);
}
