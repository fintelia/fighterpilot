
varying vec2 modelPosition;	//length will be at least 0.995 (assuming disk has 64 sides)
varying vec3 worldPosition;
varying vec2 texCoord;

varying vec3 lightDirection;
varying vec3 eyeDirection;

uniform vec2 center;
uniform float scale;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;
uniform vec3 lightPosition;

uniform vec3 eyePos;

attribute vec3 Position;

//uniform sampler3D oceanNormals;
//uniform float time;
//const float depth = 4.0; //depth of oceanNormals texture
//const float invDepth = 0.25;

void main()
{
	modelPosition = Position.xz;
	texCoord = (Position.xz*scale + center) * 0.00005;

	//position.y += mix(texture3D(oceanNormals,vec3(texCoord.xy*6.234,floor(0.0004*time*depth)*invDepth)).a, texture3D(oceanNormals,vec3(texCoord.xy*6.234,(floor(0.0004*time*depth)+1.0)*invDepth)).a,fract(0.0004*time*depth))*60.0 - 30.0;
	//position.y += mix(texture3D(oceanNormals,vec3(texCoord.xy * 128.0,floor(0.0004*time*depth)*invDepth)).a, texture3D(oceanNormals,vec3(texCoord.xy * 128.0,(floor(0.0004*time*depth)+1.0)*invDepth)).a,fract(0.0004*time*depth))*20.0 - 10.0;

	vec4 modelTransformPosition = modelTransform * vec4(Position,1.0);
	worldPosition = modelTransformPosition.xyz;

	lightDirection = lightPosition - worldPosition;
	eyeDirection = eyePos - worldPosition;

	gl_Position = cameraProjection * modelTransformPosition;
}
