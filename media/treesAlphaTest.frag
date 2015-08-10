
varying vec2 texCoord;
varying vec3 position;
varying vec3 treeColor;
uniform vec3 eyePos;

uniform sampler2D tex;
uniform samplerCube sky;


void main()
{
	vec4 color = texture2D(tex,texCoord).rgba;
	color.a *= clamp((9500.0-distance(eyePos,position))*0.002,0.0,1.0);
	if(color.a < 0.5)
	{

		discard;
	}
	else
	{
//		gl_FragColor = vec4(treeColor * color.rgb,1.0);
//		gl_FragColor = vec4(mix(color.rgb, textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),1.0);

		vec3 eyeDirection = position.xyz-eyePos;
		gl_FragColor = vec4(   mix(treeColor*color.rgb, vec3(0.5,0.5,0.5), clamp(1.0 - 100000000.0 / dot(eyeDirection,eyeDirection),0.0,0.75)) , 1.0);
	}
}