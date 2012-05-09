

varying vec3 position;
varying vec3 lightDir, halfVector;

//varying float h;

//uniform float XZscale;

//uniform float minHeight;
//uniform float maxHeight;

uniform vec3 lightPosition;
uniform vec3 eyePos;

//uniform vec3 scale;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

void main()
{
	
//	position.xz = gl_Vertex.xz/XZscale;
//	position.y = gl_Vertex.y;
	//position.xyz = gl_Vertex.xyz;

	//h = (gl_Vertex.y-0.0)/(maxHeight-0.0);
	//lightDir = normalize(lightPosition.xyz - gl_Vertex.xyz);
	//halfVector = normalize(gl_LightSource[0].halfVector.xyz);


	//eyeDirection = eyePos - gl_Vertex.xyz;
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;





	vec4 pos = modelTransform * gl_Vertex;
	position = pos.xyz;
	gl_Position = cameraProjection * pos;
	lightDir = normalize(lightPosition - position.xyz);
	halfVector = lightPosition - position.xyz;

	
}
