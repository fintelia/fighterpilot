
varying vec2 texCoord;
varying vec3 position;
varying vec4 color;

uniform samplerCube tex;
uniform sampler2D noise;
//uniform sampler2D depth;

uniform float time;

//uniform vec2 invScreenDims;

void main()
{
	//vec4 Color = vec4(texture2D(tex,texCoord).rgb,1.0);
	//vec4 c = texture2D(clouds,texCoord.xy);
	
	//float d = texture2D(depth, invScreenDims * gl_FragCoord.xy).x;
	//float alpha = clamp((d-0.995)*200.0,0.0,1.0);



	vec2 v = vec2(texCoord.x-0.5, texCoord.y-0.5);
	float s = 2.0 - 2.0*sqrt(dot(v,v) * 4.0);

	float randomIntensity =	0.5*(
							texture2D(noise, texCoord*3.0+0.00001*time*vec2(0.732546,0.680718)).r +
							texture2D(noise, texCoord*3.0+0.00001*time*vec2(-0.362358,0.932039)).r);
	//fract(texture2D(noise, texCoord*3.0+0.0001*time*vec2(0.732546,0.680718)).r+texture2D(noise, texCoord*3.0+0.0001*time*vec2(-0.3624,0.9320)).r);

	float c = clamp((exp(randomIntensity)-1.5) * s * 0.5, 0.0, 0.6);

	gl_FragColor = vec4(mix(textureCube(tex,position).rgb, vec3(0.8,0.8,0.8), c),1.0);//vec4(mix(textureCube(tex,position.xyz).rgb,vec3(1.0,1.0,1.0),c.a*0.7),1.0);
}