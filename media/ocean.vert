
varying vec3 position;
varying vec2 texCoord;

varying vec3 lightDirection;
varying vec3 eyeDirection;

uniform vec2 center;
uniform float scale;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;
uniform vec3 lightPosition;

uniform vec3 eyePos;

attribute vec3 Position;

void main()
{
	position = Position;
	texCoord = (Position.xz*scale + center) * 0.00005;

	lightDirection = normalize(lightPosition - Position);
	eyeDirection = eyePos - (modelTransform * vec4(Position,1.0)).xyz;

	gl_Position = cameraProjection * modelTransform * vec4(Position,1.0);
}
