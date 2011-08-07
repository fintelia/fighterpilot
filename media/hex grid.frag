
varying vec2 texCoord;
uniform vec4 color;

uniform sampler2D tex;
varying float h;

varying vec3 coord;

uniform float alpha;

void main()
{
	vec4 tColor = texture2D(tex,texCoord);
	float alphaMult = max((1.0 - coord.z / 50000.0) * 10.0 - 9.0, 0.0);
	float alphaMult2 = min(coord.z / 50.0,1.0);
	gl_FragColor = vec4(color.rgb, max(h*0.1, tColor.a*(h*0.1+0.15))*alphaMult*alphaMult2*color.a);
}