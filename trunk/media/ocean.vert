
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

void main()
{
	position = gl_Vertex.xyz;
	texCoord = (gl_Vertex.xz*scale + center) * 0.00005;

	lightDirection = normalize(lightPosition - gl_Vertex.xyz);
	eyeDirection = eyePos - (modelTransform * gl_Vertex).xyz;

	gl_Position = cameraProjection * modelTransform * gl_Vertex;
}
