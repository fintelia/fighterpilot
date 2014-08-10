#version 330

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D depth;

uniform vec2 invScreenDims;

in VertexData{
	vec4 color;
	vec2 texCoord;
	float invRadius;
	float depth;
}vertexIn;

const float zNear = 1.0;
const float zFar = 2000000.0;

void main()
{
	//float bufferDepth = 2.0 * zNear * zFar / (zFar + zNear - texture(depth, invScreenDims * gl_FragCoord.xy).x * (zFar - zNear));
	//float fragDepth = 2.0 * zNear * zFar / (zFar + zNear - gl_FragCoord.z * (zFar - zNear));

	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	const float Fcoef = 2.0 / log2(zFar + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;
	float logDepth =  texture2D(depth, invScreenDims * gl_FragCoord.xy).x;
	float bufferDepth = exp2(logDepth / Fcoef_half) - 1.0;

	vec4 Color = vertexIn.color * texture(tex, vertexIn.texCoord) * vec4(1.0,1.0,1.0,clamp(0.5+(bufferDepth-vertexIn.depth) * vertexIn.invRadius,0.0,1.0));
	FragColor = Color;
}
