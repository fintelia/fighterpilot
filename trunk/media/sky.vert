
varying vec2 texCoord;
varying vec4 position;

varying vec4 color;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

//uniform samplerCube tex;

void main()
{


	position = modelTransform * gl_Vertex;
	texCoord = gl_MultiTexCoord0.xy;

	gl_Position = cameraProjection * position;
	gl_Position.z = clamp(gl_Position.z,-0.5,0.5);

	//color = vec4(textureCube(tex,position.xyz).rgb,1.0);
}
