
uniform sampler2D tex;

uniform vec2 invScreenDims;
uniform vec3 eyePos;


varying vec3 position;
varying vec2 texCoord;
varying vec3 normal;
varying vec3 eyeDirection;

void main()
{
	vec2 v = vec2(texCoord.x-0.5, texCoord.y-0.5);
	float s = 2.0 - 2.0*sqrt(dot(v,v) * 4.0);

	float dist = length(position-eyePos);

	float t = min(dist*0.1, 1.0) * min(1000.0/dist, 1.0) * 5.0;

	gl_FragColor = vec4(1.0,1.0,1.0, clamp(exp(texture2D(tex, texCoord*3.0).r)-1.5, 0.0, 1.0) * s * t);
}