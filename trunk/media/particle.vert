
varying vec2 texCoord;
varying vec4 color;

uniform mat4 cameraProjection;

void main()
{
	texCoord = gl_MultiTexCoord0.xy;
	color = gl_Color;

	gl_Position = cameraProjection * gl_Vertex;
}
