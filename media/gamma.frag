
uniform sampler2D tex;
varying float g;

varying vec2 texCoord;

void main()
{
	vec3 t = texture2D(tex,texCoord).rgb;
	//t /= t + 0.5; //tone mapping
	gl_FragColor = vec4(pow(t.x,g),pow(t.y,g),pow(t.z,g),1.0);
}