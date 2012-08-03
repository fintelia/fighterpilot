
varying vec2 texCoord;
varying vec3 position;
uniform vec3 eyePos;

uniform sampler2D tex;
uniform samplerCube sky;


void main()
{
	vec4 color = texture2D(tex,texCoord).rgba;
	if(color.a < 0.999)
	{
		discard;
	}
	else
	{
		vec3 eyeDirection = position-eyePos;
		gl_FragColor = vec4(mix(color.rgb, textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),1.0);
	}
}