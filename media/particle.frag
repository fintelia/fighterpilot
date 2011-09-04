
uniform sampler2D tex;
uniform sampler2D depth;

uniform vec2 invScreenDims;

//varying float depthVal;
varying vec2 texCoord;
varying vec4 color;


void main()
{
	float d = texture2D(depth, invScreenDims * gl_FragCoord.xy).x;
	gl_FragColor = color * texture2D(tex, texCoord) * vec4(1.0,1.0,1.0,clamp((d-gl_FragCoord.z)*1000.0,0.0,1.0));// * vec4(1.0,1.0,1.0,clamp(texture2D(depth, gl_FragCoord.xy).x - depthVal,0.0,1.0));
}