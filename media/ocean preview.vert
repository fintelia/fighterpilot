#version 120

varying vec3 position, lightDir, halfVector;
varying vec2 texCoord;

uniform float XZscale;

void main()
{
	position.y = 0.0;
	position.xz = gl_Vertex.xz/XZscale;

	lightDir = normalize(gl_LightSource[0].position.xyz);
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	gl_Position = ftransform();
}