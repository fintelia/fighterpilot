
varying vec2 texCoord;
//varying vec3 normal;
//varying vec4 color;
//varying vec3 lightPosition;
//varying vec4 position;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;
void main()
{
	texCoord = gl_MultiTexCoord0.xy;
//	normal = gl_Normal;
//	color = gl_Color;

	//position = modelTransform * gl_Vertex;
	gl_Position = /*ftransform();//*/cameraProjection * modelTransform * gl_Vertex;
	gl_Position.z = clamp(gl_Position.z,-0.5,0.5);

//	lightPosition = normalize(gl_LightSource[0].position.xyz);
}
