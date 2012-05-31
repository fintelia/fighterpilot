
varying vec2 texCoord;
varying vec3 position;

uniform samplerCube tex;
uniform sampler2D clouds;
varying vec4 color;

void main()
{
	//vec4 Color = vec4(texture2D(tex,texCoord).rgb,1.0);
	//vec4 c = texture2D(clouds,texCoord.xy);
	gl_FragColor = vec4(textureCube(tex,position).rgb,1.0);//vec4(mix(textureCube(tex,position.xyz).rgb,vec3(1.0,1.0,1.0),c.a*0.7),1.0);
}