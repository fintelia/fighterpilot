
uniform sampler2D tex;
uniform sampler2D depth;

uniform vec2 invScreenDims;

varying vec2 texCoord;
varying vec4 color;


const float zNear = 1.0;
const float zFar = 500000;

void main()
{
	float bufferDepth = 2.0 * zNear * zFar / (zFar + zNear - texture2D(depth, invScreenDims * gl_FragCoord.xy).x * (zFar - zNear));
	float fragDepth = 2.0 * zNear * zFar / (zFar + zNear - gl_FragCoord.z * (zFar - zNear));

	vec4 Color = color * texture2D(tex, texCoord) * vec4(1.0,1.0,1.0,clamp(0.5+(bufferDepth-fragDepth),0.0,1.0));
	gl_FragColor = Color;
}