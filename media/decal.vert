
uniform mat4 cameraProjection;
uniform mat4 modelTransform;

attribute vec3 Position;
attribute vec2 TexCoord;

varying vec2 texCoord;

void main()
{
	texCoord = TexCoord;
	gl_Position = cameraProjection * modelTransform * vec4(Position,1.0);
}
