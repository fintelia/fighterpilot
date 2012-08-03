
varying vec2 texCoord;
varying vec3 position;
varying vec3 normal;
varying vec3 eyeDirection;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;
uniform vec3 eyePos;

attribute vec3 Position;
attribute vec3 Normal;
attribute vec2 TexCoord;

void main()
{
	position = Position;
	texCoord = TexCoord;
	normal = (modelTransform * vec4(Normal,0.0)).xyz;
	eyeDirection = position - eyePos;

	gl_Position = cameraProjection * modelTransform * vec4(position,1.0);
}
