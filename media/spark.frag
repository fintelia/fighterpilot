
uniform sampler2D tex;
uniform sampler2D depth;

uniform vec2 invScreenDims;

varying vec2 texCoord;
varying vec4 color;
varying float flogz;

const float zNear = 1.0;
const float zFar = 2000000.0;

void main()
{
	//see: http://outerra.blogspot.com/2013/07/logarithmic-depth-buffer-optimizations.html
	const float Fcoef = 2.0 / log2(zFar + 1.0);
	const float Fcoef_half = 0.5 * Fcoef;
	float logDepth =  texture2D(depth, invScreenDims * gl_FragCoord.xy).x;
	float bufferDepth = exp2(logDepth / Fcoef_half) - 1.0;
	float fragDepth = log2(flogz) * Fcoef_half;

	vec4 Color = color * texture2D(tex, texCoord) * vec4(1.0,1.0,1.0,clamp(0.5+(bufferDepth-fragDepth),0.0,1.0));
	gl_FragColor = Color;
}
