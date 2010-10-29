
varying vec3 position, lightDir, halfVector;

uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;

void main()
{
	if(position.x < 0.0 || position.x > 1.0 || position.z < 0.0 || position.z > 1.0) discard;
	gl_FragColor = texture2D(snow,position.xz*20.0) * (1.1-texture2D(LCnoise,position.xz*20.0*5.0).r*0.2);
}