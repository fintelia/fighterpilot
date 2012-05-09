
varying vec4 position;
varying vec3 lightDir, halfVector;

uniform vec3 lightPosition;
uniform mat4 cameraProjection;
uniform mat4 modelTransform;

void main()
{
	position = modelTransform * gl_Vertex;
	gl_Position = cameraProjection * position;

	halfVector = lightPosition - position.xyz;
	lightDir = normalize(lightPosition - position.xyz);
}