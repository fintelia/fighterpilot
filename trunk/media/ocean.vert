
varying vec3 lightDir;
varying vec2 position;
varying vec2 texCoord;
varying float depth;

uniform vec2 center;
uniform float scale;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;
uniform vec3 lightPosition;

void main()
{
	lightDir = normalize(lightPosition - gl_Vertex.xyz);
	texCoord = (gl_Vertex.xz*scale + center) * 0.00005;
	position = gl_Vertex.xz;
	gl_Position = cameraProjection * modelTransform * gl_Vertex;
	depth = gl_Position.w;
} 
