
varying vec2 texCoord;
varying vec3 normal;
varying vec4 color;
varying vec3 lightDir;
varying vec4 position;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

uniform vec3 lightPosition;

void main()
{
	texCoord = gl_MultiTexCoord0.xy;
	normal = modelTransform * vec4(gl_Normal,0.0);
	color = gl_Color;

	position = modelTransform * gl_Vertex;
	gl_Position = cameraProjection * position;
	

	lightDir = normalize(lightPosition - position.xyz);
}