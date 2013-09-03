// vertex shader
#version 400

//varyings
//out VertexData{
//	vec3 worldPosition;
//	vec3 sunDir, sunHalfVector;
//	vec3 lightDirections[4];
//}vData;
struct VertexData{
	vec3 worldPosition;
	vec3 sunDir, sunHalfVector;
	vec3 lightDirections[4];
};
out VertexData vData;

//uniforms
uniform vec3 sunPosition;
uniform vec3 lightPositions[4];
uniform vec3 eyePos;
uniform mat4 cameraProjection;
uniform mat4 modelTransform;

//attributes
in vec3 Position;
 
void main()
{
	vec4 pos = modelTransform * vec4(Position,1.0);
	vData.worldPosition = pos.xyz;

	vData.sunHalfVector = sunPosition + vData.worldPosition.xyz;
	vData.sunDir = normalize(sunPosition - vData.worldPosition.xyz);


	vData.lightDirections[0] = normalize(lightPositions[0] - vData.worldPosition.xyz);
	vData.lightDirections[1] = normalize(lightPositions[1] - vData.worldPosition.xyz);
	vData.lightDirections[2] = normalize(lightPositions[2] - vData.worldPosition.xyz);
	vData.lightDirections[3] = normalize(lightPositions[3] - vData.worldPosition.xyz);
}