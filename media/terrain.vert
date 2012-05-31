

varying vec3 position;
varying vec3 lightDir, halfVector;

//varying float h;

//uniform float XZscale;

//uniform float minHeight;
//uniform float maxHeight;

uniform vec3 lightPosition;
uniform vec3 eyePos;

//uniform vec3 scale;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

attribute vec3 Position;

void main()
{
	vec4 pos = modelTransform * vec4(Position,1.0);
	position = pos.xyz;
	gl_Position = cameraProjection * pos;
	lightDir = normalize(lightPosition - position.xyz);
	halfVector = lightPosition - position.xyz;
}
