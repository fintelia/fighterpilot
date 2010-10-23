
varying vec3 position, lightDir, halfVector;
varying float seaLevel;

uniform float time;

uniform sampler2D sand;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;
uniform sampler2D LCnoise;
uniform sampler2D waterBumpMap;

void main()
{
	gl_FragColor = texture2D(snow,position.xz*20.0) * (1.1-texture2D(LCnoise,position.xz*20.0*5.0).r*0.2);
}