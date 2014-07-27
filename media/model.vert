
varying vec2 texCoord;
varying float flogz;
//varying vec3 normal;

centroid varying vec4 position;
centroid varying vec3 lightDir;
centroid varying vec3 halfVector;

uniform mat4 cameraProjection;
uniform mat4 modelTransform;

uniform vec3 lightPosition;
uniform vec3 eyePosition;

attribute vec3 Position;
attribute vec2 TexCoord;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;

//varying vec3 b;

varying vec3 normal;
varying vec3 eyeDirection;

void main()
{
	texCoord = TexCoord;
	//normal = (modelTransform * vec4(Normal,0.0)).xyz;

//	vec3 normal = normalize((modelTransform * vec4(Normal,0.0)).xyz);
//	vec3 tangent = normalize((modelTransform * vec4(Tangent,0.0)).xyz);
//	vec3 bitangent = normalize((modelTransform * vec4(Bitangent,0.0)).xyz);//normalize(cross(normal, tangent));

	//tangent = normalize(cross(bitangent, normal));
	//bitangent = normalize(cross(normal, tangent));

	//mat3 tbnMatrix = mat3(tangent.x, bitangent.x, normal.x,
    //                     tangent.y, bitangent.y, normal.y,
    //                    tangent.z, bitangent.z, normal.z);
	
//	mat3 tbnMatrix = mat3(tangent,bitangent,normal);
	
	position = modelTransform * vec4(Position,1.0);
	gl_Position = cameraProjection * position;

	eyeDirection = normalize(eyePosition - position.xyz);
	halfVector = /*tbnMatrix * */ (lightPosition.xyz + position.xyz);
	lightDir = /*tbnMatrix * */ (lightPosition/* - position.xyz*/);

	//b = tangent;

	normal = (modelTransform * vec4(Normal,0)).xyz;

	flogz = 1.0 + gl_Position.w;
}
