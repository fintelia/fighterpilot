

varying vec3 position, eyeDirection, lightDir, halfVector;
varying float h;

uniform float XZscale;

uniform float minHeight;
uniform float maxHeight;

uniform vec3 lightPosition;
uniform vec3 eyePos;

void main()
{
	
	position.xz = gl_Vertex.xz/XZscale;
	position.y = gl_Vertex.y;
	h = (gl_Vertex.y-0)/(maxHeight-0);
	//gl_Vertex.y *= 0.1;
	lightDir = normalize(lightPosition.xyz - gl_Vertex.xyz);
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	//vec4 v=vec4(gl_Vertex.x,max(gl_Vertex.y,0.0),gl_Vertex.zw);

	eyeDirection = eyePos - gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
