
varying vec2 texCoord;
varying vec3 position;
varying vec3 treeColor;

uniform vec3 eyePos;

uniform sampler2D tex;
uniform samplerCube sky;


void main()
{
	vec4 color = texture2D(tex,texCoord).rgba;
	if(color.a < 0.0001)
	{
		discard;
	}
	else
	{
		color.a *= clamp((9500.0-distance(eyePos,position))*0.02,0.0,1.0);
		vec3 eyeDirection = position-eyePos;
		float eyeDist = length(eyeDirection);
		//gl_FragColor = vec4(treeColor,1.0) * color.rgba /* clamp((9500.0-distance(eyePos,position))*0.002,0.0,1.0)*/;//vec4(mix(color.rgb, textureCube(sky, vec3(-eyeDirection.x,0,-eyeDirection.z)).rgb, clamp(0.000000001*dot(eyeDirection,eyeDirection),0.0,1.0)),color.a /* clamp((10000.0-distance(eyePos,position))*0.002,0.0,1.0)*/);

		vec3 fogColor = vec3(0.5, 0.5, 0.5);
		float fogAmount = 1-min(5000.0/length(eyePos - position), 1);
			// clamp(
		    // 1.0 - 100000000.0 / dot(eyeDirection, eyeDirection), 0.0, 0.75);

		gl_FragColor.rgb = mix(treeColor * color.rgb, fogColor, fogAmount);
		gl_FragColor.a = color.a * clamp(1 - 0.001*(eyeDist - 6000), 0, 1);
	}
}
