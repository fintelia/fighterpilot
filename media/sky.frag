
varying vec2 texCoord;
varying vec3 position;

uniform samplerCube tex;
uniform sampler2D noise;
varying vec4 color;

void main()
{
	//vec4 Color = vec4(texture2D(tex,texCoord).rgb,1.0);
	//vec4 c = texture2D(clouds,texCoord.xy);
	
	vec2 v = vec2(texCoord.x-0.5, texCoord.y-0.5);
	float s = 2.0 - 2.0*sqrt(dot(v,v) * 4.0);
	float c = clamp((exp(texture2D(noise, texCoord*3.0).r)-1.5) * s * 0.5, 0.0, 0.6);

	gl_FragColor = vec4(mix(textureCube(tex,position).rgb, vec3(0.8,0.8,0.8), c),1.0);//vec4(mix(textureCube(tex,position.xyz).rgb,vec3(1.0,1.0,1.0),c.a*0.7),1.0);
}