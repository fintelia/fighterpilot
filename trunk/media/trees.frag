
varying vec2 texCoord;
varying vec3 position;
varying vec3 treeColor;

uniform vec3 eyePos;

uniform sampler2D tex;
uniform samplerCube sky;


void main()
{
	//gl_FragColor = vec4(1,0,0,1);
	//return;


	vec4 color = texture2D(tex,texCoord).rgba;
	if(color.a < 0.0001)
	{
		discard;
	}
	else
	{
		vec3 eyeDirection = position-eyePos;
		float eyeDist = length(eyeDirection);
		gl_FragColor = vec4(treeColor,1.0) * color.rgba * clamp((9500.0-distance(eyePos,position))*0.002,0.0,1.0);//vec4(mix(color.rgb, textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a /* clamp((10000.0-distance(eyePos,position))*0.002,0.0,1.0)*/);
	}
}