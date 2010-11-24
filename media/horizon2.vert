
varying vec3 lightDir, halfVector;
varying float height, rad;

uniform float heightRange;
uniform vec2 center;

void main()
{	
	lightDir = normalize(vec3(gl_LightSource[0].position));
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	gl_TexCoord[0] = gl_MultiTexCoord0;
	height=(gl_Vertex.y/heightRange+0.5);

	rad = 1.0;
	if((center.x-gl_Vertex.x)+(center.y-gl_Vertex.z) < 0.1)	rad = 0.0;

	gl_Position = ftransform();
} 