

varying vec3 position, lightDir, halfVector;
varying float seaLevel;

uniform float XZscale;
uniform float minHeight;
uniform float maxHeight;
uniform float sea_level;

void main()
{
	position.xz = gl_Vertex.xz/XZscale/15.0;
	position.y = (gl_Vertex.y-minHeight)/(maxHeight-minHeight);
	lightDir = normalize(vec3(gl_LightSource[0].position));
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	seaLevel = (sea_level-minHeight)/(maxHeight-minHeight);

	vec4 v=vec4(gl_Vertex.x,max(gl_Vertex.y,0.0),gl_Vertex.zw);
	gl_Position = gl_ModelViewProjectionMatrix * v;
}
