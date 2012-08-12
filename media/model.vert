
varying vec2 texCoord;
//varying vec3 normal;

centroid varying vec4 position;
centroid varying vec3 lightDir;
centroid varying vec3 halfVector;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

uniform vec3 lightPosition;

attribute vec3 Position;
attribute vec2 TexCoord;
attribute vec3 Normal;
attribute vec3 Tangent;

void main()
{
	texCoord = TexCoord;
	//normal = (modelTransform * vec4(Normal,0.0)).xyz;

	vec3 normal = normalize((modelTransform * vec4(Normal,0.0)).xyz);
	vec3 tangent = (modelTransform * vec4(Tangent,0.0)).xyz;
	vec3 bitangent = cross(normal, tangent);

	tangent = normalize(cross(bitangent, normal));
	bitangent = normalize(cross(normal, tangent));

	mat3 tbnMatrix = mat3(tangent.x, bitangent.x, normal.x,
                          tangent.y, bitangent.y, normal.y,
                          tangent.z, bitangent.z, normal.z);

	position = modelTransform * vec4(Position,1.0);
	gl_Position = cameraProjection * position;

	halfVector = tbnMatrix * (lightPosition - position.xyz);
	lightDir = tbnMatrix * normalize(lightPosition - position.xyz);
}
