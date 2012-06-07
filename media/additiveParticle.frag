
uniform sampler2D tex;
uniform sampler2D depth;

uniform vec2 invScreenDims;

varying vec2 texCoord;
varying vec4 color;


void main()
{
	float d = texture2D(depth, invScreenDims * gl_FragCoord.xy).x;
	vec4 Color = color * texture2D(tex, texCoord) * vec4(1.0,1.0,1.0,clamp((d-gl_FragCoord.z)*1000.0,0.0,1.0));
	gl_FragColor = Color;
}