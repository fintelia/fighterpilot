#version 330

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D depth;

uniform vec2 invScreenDims;

in VertexData{
	vec4 color;
	vec2 texCoord;
	float invRadius;
}vertexIn;

const float zNear = 1.0;
const float zFar = 500000;

void main()
{
	float bufferDepth = 2.0 * zNear * zFar / (zFar + zNear - texture(depth, invScreenDims * gl_FragCoord.xy).x * (zFar - zNear));
	float fragDepth = 2.0 * zNear * zFar / (zFar + zNear - gl_FragCoord.z * (zFar - zNear));

	vec4 Color = vertexIn.color * texture(tex, vertexIn.texCoord) * vec4(1.0,1.0,1.0,clamp(0.5+(bufferDepth-fragDepth) * vertexIn.invRadius,0.0,1.0));
	FragColor = Color;
}