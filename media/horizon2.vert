
varying vec3 lightDir, halfVector;
//varying float rad;
varying vec2 position;
varying vec2 texCoord;
uniform vec2 center;
uniform float scale;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

void main()
{
	lightDir = normalize(vec3(gl_LightSource[0].position));
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	//gl_TexCoord[0] = gl_MultiTexCoord0;
	texCoord = (gl_Vertex.xz*scale + center) * 0.00005;

	position = gl_Vertex.xz;
	//rad = 1.0;
	//if((center.x-gl_Vertex.x)*(center.x-gl_Vertex.x)+(center.y-gl_Vertex.z)*(center.y-gl_Vertex.z) < 0.01)	rad = 0.0;

	gl_Position = cameraProjection * modelTransform * gl_Vertex;
	//pos=gl_Vertex.xyz;
} 
