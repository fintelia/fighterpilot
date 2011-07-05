
varying vec2 texCoord;
varying vec3 normal;
varying vec4 color;
varying vec3 lightPosition;

void main()
{
	texCoord = gl_MultiTexCoord0.xy;
	normal = gl_Normal;
	color = gl_Color;
	gl_Position = ftransform();

	lightPosition = normalize(gl_LightSource[0].position.xyz);
}
