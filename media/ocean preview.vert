
varying vec3 position, lightDir, halfVector;
varying vec2 texCoord;

uniform float XZscale;
//uniform vec3 lightPosition;

void main()
{
	position.y = 0.0;
	position.xz = gl_Vertex.xz/XZscale;

	//lightDir = normalize(gl_LightSource[0].position.xyz);
	lightDir = vec3(0.6,0.8,0.0);//normalize(lightPosition.xyz - gl_Vertex.xyz);
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	gl_Position = ftransform();
}