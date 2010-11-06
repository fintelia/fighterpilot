

varying vec3 position, lightDir, halfVector;

uniform float XZscale;

void main()
{
	position.xz = gl_Vertex.xz/XZscale;
	position.y = 0.0;

	lightDir = normalize(gl_LightSource[0].position.xyz);
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	gl_Position = ftransform();
}
