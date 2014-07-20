

varying vec3 position;
varying vec3 sunDir, sunHalfVector;
varying vec3 lightDirections[4];

//varying float h;

//uniform float XZscale;

//uniform float minHeight;
//uniform float maxHeight;

uniform vec3 sunPosition;
uniform vec3 lightPositions[4];
uniform vec3 eyePos;

//uniform vec3 scale;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

attribute vec3 Position;

//uniform int numLights = 0;

void main()
{
	vec4 pos = modelTransform * vec4(Position,1.0);
	position = pos.xyz;
	gl_Position = cameraProjection * pos;

	sunHalfVector = sunPosition + position.xyz;
	sunDir = normalize(sunPosition/* - position.xyz*/);


	lightDirections[0] = normalize(lightPositions[0] - position.xyz);
	lightDirections[1] = normalize(lightPositions[1] - position.xyz);
	lightDirections[2] = normalize(lightPositions[2] - position.xyz);
	lightDirections[3] = normalize(lightPositions[3] - position.xyz);
	//for(int i=0; i<numLights; i++)
	//{
	//	lightDir[i] = normalize(lightPosition[i] - position.xyz);
	//}
}
