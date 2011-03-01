
#version 120

uniform sampler2D tex;
varying float alpha;

void main()
{
	gl_FragColor = vec4(0.55,0.55,0.6,alpha)*texture2D(tex,gl_PointCoord.xy);
}