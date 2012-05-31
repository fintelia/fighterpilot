
varying vec4 position;
varying vec3 lightDir, halfVector;

uniform vec3 lightPosition;
uniform mat4 cameraProjection;
uniform mat4 modelTransform;

attribute vec3 Position;

void main()
{
	position = modelTransform * vec4(Position.xyz,1.0);
	gl_Position = cameraProjection * position;

	halfVector = lightPosition - position.xyz;
	lightDir = normalize(lightPosition - position.xyz);
}