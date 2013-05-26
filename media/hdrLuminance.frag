
uniform sampler2D tex;
varying vec2 texCoord;

const float delta = 1e-6;

//see: http://gamedev.stackexchange.com/questions/50175/hdr-tone-mapping-choosing-parameters

void main()
{
	vec3 color = texture(tex, texCoord).xyz;
	float luminance = dot(color, vec3(0.2125, 0.7154, 0.0721));
	float logLuminance = log(delta + luminance);
	gl_FragColor = vec4(logLuminance, logLuminance, 0.0, 0.0); //we could have the first channel stores max luminance during a minification process
}